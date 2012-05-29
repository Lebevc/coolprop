#ifndef R134a_H
#define R134a_H

	class R134aClass : public Fluid{

	public:
		R134aClass();
		~R134aClass(){};
		virtual double conductivity_Trho(double, double);
		virtual double viscosity_Trho(double, double);
		double psat(double);
		double rhosatL(double);
		double rhosatV(double);

		double viscosity_background(double T, double rho);
		double viscosity_dilute(double T);
		double viscosity_residual(double T, double rho);
		void ECSParams(double *e_k, double *sigma);
	};
#endif
