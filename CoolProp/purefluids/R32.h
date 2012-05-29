#ifndef R32_H
#define R32_H

	class R32Class : public Fluid{

	public:
		R32Class();
		~R32Class(){};
		virtual double conductivity_Trho(double, double);
		virtual double viscosity_Trho(double, double);
		double psat(double);
		double rhosatL(double);
		double rhosatV(double);

		void ECSParams(double *e_k, double *sigma);
		double ECS_psi_viscosity(double rhor);
	};
#endif
