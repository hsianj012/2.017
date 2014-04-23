#ifndef HMC6352_H
#define HMC6352_H

class HMC6352
{
	public:
		enum operationalMode 
		{  
			STANDBY = 0,
			QUERY = 1,
			CONTINUOUS = 2,
			UNKNOWN = 3
		};

		// void constructor
		HMC6352();	

		// user-specified address
		// CAREFUL: as we're using wire, it assumes 7-bit addresses, so 
		// 8 bit addresses need to be right-shifted one bit (e.g, the 
		// factury default of 0x42 needs to be written as 0x21)
		HMC6352(int address);

		float getHeading(void);

		int getOperationalMode(void);

		int getOutputMode(void);
		void setOutputMode(const int& mode);

	private:
		// commands (generic)		
		static const int c_writeToEEPROM_ 	= 0x77;	// w (write)
		static const int c_readFromEEPROM_	= 0x72;	// r (read)
		static const int c_writeToRAM_		= 0x47;	// G
		static const int c_readFromRAM_		= 0x67;	// g
		// commands (mode-specific)
		static const int c_getDataStandby_ 	= 0x41;

		// adresses
		static const int c_operationalModeAddressRAM_		= 0x74;
		static const int c_operationalModeAddressEEPROM_	= 0x08;
		static const int c_outputModeAddressRAM_			= 0x4E;
		// output mode configuration is NOT shadowed in RAM

		int address_;
};

#endif