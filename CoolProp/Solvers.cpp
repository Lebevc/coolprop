#include <vector>
#include "Solvers.h"
#include "math.h"
#include <Eigen/Dense>
#include <iostream>
#include "PropMacros.h"
#include "CPExceptions.h"
/**
In this formulation of the Multi-Dimensional Newton-Raphson solver the Jacobian matrix is known.
Therefore, the dx vector can be obtained from 

J(x)dx=-f(x)

for a given value of x.  The pointer to the class FuncWrapperND that is passed in must implement the call() and Jacobian() 
functions, each of which take the vector x. The data is managed using the Eigen class which is described in 
http://eigen.tuxfamily.org/dox/TutorialLinearAlgebra.html.

All the linear algebra functions are provided by the Eigen package. (see link above)

@param f A pointer to an instance of the FuncWrapperND class that implements the call() and Jacobian() functions
@param x0 The initial guess value for the solution
@param tol The root-sum-square of the errors from each of the components
@param maxiter The maximum number of iterations
@param errstring  A string with the returned error.  If the length of errstring is zero, no errors were found
@returns If no errors are found, the solution.  Otherwise, _HUGE, the value for infinity
*/
Eigen::VectorXd NDNewtonRaphson_Jacobian(FuncWrapperND *f, Eigen::VectorXd x0, double tol, int maxiter, std::string *errstring)
{
	int iter=0;
	*errstring=std::string("");
	Eigen::VectorXd f0;
	Eigen::MatrixXd J;
	double error = 999;
	while (iter==0 || fabs(error)>tol){
		
		double T =  x0(0), rho = x0(1);
		f0 = f->call(x0);
		J = f->Jacobian(x0);
		x0 -= J.inverse()*f0;
		error = sqrt(f0.array().square().sum());
		//std::cout << J << std::endl << x0 << std::endl << error << std::endl;
		if (iter>maxiter){
			*errstring=std::string("reached maximum number of iterations");
			x0(0)=_HUGE;
		}
		iter++;
	}
	return x0;
}

//double NDNewtonRaphson(FuncWrapperND *f,std::vector<double> x0, double tol, int maxiter, std::string *errstring)
//{
//	/*
//	In this formulation, no Jacobian function is provided, thus we must calculate it numerically
//	http://eigen.tuxfamily.org/dox/TutorialLinearAlgebra.html
//	*/
//	std::vector<double> f0;
//	f0=f->call();
//
//}

/**
In the secant function, a 1-D Newton-Raphson solver is implemented.  An initial guess for the solution is provided.

@param f A pointer to an instance of the FuncWrapper1D class that implements the call() function 
@param x0 The inital guess for the solutionh
@param dx The initial amount that is added to x in order to build the numerical derivative
@param tol The absolute value of the tolerance accepted for the objective function
@param maxiter Maximum number of iterations
@param errstring A pointer to the std::string that returns the error from Secant.  Length is zero if no errors are found
@returns If no errors are found, the solution, otherwise the value _HUGE, the value for infinity
*/
double Secant(FuncWrapper1D *f, double x0, double dx, double tol, int maxiter, std::string *errstring)
{
	double x1=0,x2=0,x3=0,y1=0,y2=0,x,fval=999;
    int iter=1;
	*errstring=std::string("");
	
	if (fabs(dx)==0){ *errstring=std::string("dx cannot be zero"); return _HUGE;}
    while ((iter<=3 || fabs(fval)>tol) && iter<100)
    {
        if (iter==1){x1=x0; x=x1;}
        if (iter==2){x2=x0+dx; x=x2;}
        if (iter>2) {x=x2;}
			fval=f->call(x);
        if (iter==1){y1=fval;}
        if (iter>1)
        {
            y2=fval;
            x3=x2-y2/(y2-y1)*(x2-x1);
            y1=y2; x1=x2; x2=x3;
			
        }
		if (iter>maxiter)
		{
			*errstring=std::string("reached maximum number of iterations");
			return _HUGE;
		}
        iter=iter+1;
    }
    return x3;
}

/** 

This function implements a 1-D bounded solver using the algorithm from Brent, R. P., Algorithms for Minimization Without Derivatives.
Englewood Cliffs, NJ: Prentice-Hall, 1973. Ch. 3-4.

a and b must bound the solution of interest and f(a) and f(b) must have opposite signs.  If the function is continuous, there must be
at least one solution in the interval [a,b].

@param f A pointer to an instance of the FuncWrapper1D class that must implement the class() function
@param a The minimum bound for the solution of f=0
@param b The maximum bound for the solution of f=0
@param macheps The machine precision
@param t Tolerance (absolute)
@param maxiter Maximum numer of steps allowed.  Will throw a SolutionError if the solution cannot be found
@param errstr A pointer to the error string returned.  If length is zero, no errors found.
*/
double Brent(FuncWrapper1D *f, double a, double b, double macheps, double t, int maxiter, std::string *errstr)
{
	int iter;
	*errstr=std::string("");
	double fa,fb,c,fc,m,tol,d,e,p,q,s,r;
    fa=f->call(a);
    fb=f->call(b);
    c=a;
    fc=fa;
	iter=1;
	if (fabs(fc)<fabs(fb)){
        // Goto ext: from Brent ALGOL code
        a=b;
        b=c;
        c=a;
        fa=fb;
        fb=fc;
        fc=fa;
	}
    d=b-a;
    e=b-a;
    m=0.5*(c-b);
    tol=2*macheps*fabs(b)+t;
	while (fabs(m)>tol && fb!=0){
        // See if a bisection is forced
		if (fabs(e)<tol || fabs(fa) <= fabs(fb)){
            m=0.5*(c-b);
            d=e=m;
		}
		else{
            s=fb/fa;
			if (a==c){
                //Linear interpolation
                p=2*m*s;
                q=1-s;
			}
			else{
                //Inverse quadratic interpolation
                q=fa/fc;
                r=fb/fc;
                m=0.5*(c-b);
                p=s*(2*m*q*(q-r)-(b-a)*(r-1));
                q=(q-1)*(r-1)*(s-1);
			}
			if (p>0){
				q=-q;
			}
			else{
				p=-p;
			}
            s=e;
            e=d;
            m=0.5*(c-b);
			if (2*p<3*m*q-fabs(tol*q) || p<fabs(0.5*s*q)){
                d=p/q;
			}
			else{
                m=0.5*(c-b);
                d=e=m;
			}
		}
        a=b;
        fa=fb;
		if (fabs(d)>tol){
            b+=d;
		}
        else if (m>0){
            b+=tol;
		}
		else{
            b+=-tol;
		}
		fb=f->call(b);
		if (fb*fc>0){
            // Goto int: from Brent ALGOL code
            c=a;
            fc=fa;
            d=e=b-a;
		}
		if (fabs(fc)<fabs(fb)){
            // Goto ext: from Brent ALGOL code
            a=b;
            b=c;
            c=a;
            fa=fb;
            fb=fc;
            fc=fa;
		}
        m=0.5*(c-b);
        tol=2*macheps*fabs(b)+t;
		iter+=1;
		if (iter>maxiter){
			throw SolutionError(std::string("Reached maximum number of steps")); 
			return _HUGE;
		}
	}
    return b;
}