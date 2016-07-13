
#include "PFManagerPrivate.hpp"

class PFMService
{
	public:
		PFMService() {};
		virtual ~PFMService() {};


};

class PFManager
{
	public:
		PFManager() {};
		~PFManager() {};
		void display(FILE* fp)
		{
			fprintf(fp, "debug_: [%d]\n", debug_);
		}

	private:
		bool debug_;

		bool loadConfiguration(
				const string& inifile)
		{
			//if(! PFMProfile::getValueString(inifile, string("PFManager"), string("debug"), debug_) )
			//{
			//	return false;
			//}

			return true;
		}
};

int main(void)
{
	PFManager pfm;

	pfm.display(stdout);
	return 0;
}

