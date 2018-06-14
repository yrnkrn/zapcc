#pragma once
namespace QuantLib {
struct Observer {                       
	virtual void update() = 0;       
	         int name;         
};

struct TermStructure : public virtual Observer {       	                
        void update();         
}; 

struct VolatilityTermStructure : public TermStructure {       
		 void checkStrike() const;
};   

struct SwaptionVolatilityStructure : public VolatilityTermStructure {       
	virtual int volatilityImpl() const;         
};

class LazyObject {
  virtual void performCalculations() const = 0;
};

struct SwaptionVolatilityDiscrete : public LazyObject, public SwaptionVolatilityStructure {
      void update();
};
}
