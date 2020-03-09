#pragma once
class Currency
{
    public:
        void ReadCurrency();
        float GetExchangeRate24SaleRateNB() const
        {
            return exchangeRate_24_saleRateNB;
        }
        float GetExchangeRate24PurchaseRateNB() const
        {
            return exchangeRate_24_purchaseRateNB;
        }
        float GetExchangeRate24SaleRate() const
        {
            return exchangeRate_24_saleRate;
        }
        float GetExchangeRate24PurchaseRate() const
        {
            return exchangeRate_24_purchaseRate;
        }
    private:
        float exchangeRate_24_saleRateNB {0.0f};
        float exchangeRate_24_purchaseRateNB {0.0f};
        float exchangeRate_24_saleRate {0.0f};
        float exchangeRate_24_purchaseRate {0.0f};
};