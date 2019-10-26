#ifndef CPU_H
#define CPU_H

#include <stdio.h>
#include <sys/types.h>
#include "mem-sim.h"

class CPU {
  private:
	FILE* _trace;

	Request _currReq;

	cpuState _state; 

	u_int32_t _readyAt;

	bool _done; 

	u_int32_t nRequests; 
	u_int32_t hitsL1; 
	u_int32_t hitsL2; 
	u_int64_t totalAccessTime;

  public:
	CPU(char* trace_file); 

	~CPU();

	bool isDone(); 

	void readNextRequest(u_int32_t cycle);

	Request issueRequest(u_int32_t cycle); 

	Request getRequest(); 


	void completeRequest(u_int32_t cycle);

	void hitL1(bool isHit);

	void loadHitL2(bool isHit);
	void storeHitL2(bool isHit);


	cpuState getStatus(u_int32_t cycle); 


	void setStatus(cpuState new_state);

	double getHitRateL1(); 
	double getHitRateL2();
	double getAMAT();
};

#endif
