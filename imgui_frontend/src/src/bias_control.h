#ifndef BIAS_CONTROL_H
#define BIAS_CONTROL_H
class AdcControl{
    public:
        void ShowAdcControlCenter(bool* p_open);
        bool saveBiases(char);
        bool loadBiases();
        void showSaveLoadBiases();
        void showSaveLoadBiasesPopup();
};
#endif