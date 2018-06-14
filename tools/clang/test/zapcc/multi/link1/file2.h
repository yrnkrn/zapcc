#pragma once
namespace QuantLib {
struct SwaptionVolatilityMatrix : public SwaptionVolatilityDiscrete {         
        void performCalculations() const;         
};
}
