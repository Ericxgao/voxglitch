#include "AllSampleAndHoldsValueItem.hpp"

struct AllSampleAndHoldsItem : MenuItem {
  DigitalSequencer *module;

  Menu *createChildMenu() override {
    Menu *menu = new Menu;

    dseq::AllSampleAndHoldsValueItem *sample_and_hold_off = createMenuItem<dseq::AllSampleAndHoldsValueItem>("Off");
    sample_and_hold_off->module = module;
    sample_and_hold_off->value = false;
    menu->addChild(sample_and_hold_off);

    dseq::AllSampleAndHoldsValueItem *sample_and_hold_on = createMenuItem<dseq::AllSampleAndHoldsValueItem>("On");
    sample_and_hold_on->module = module;
    sample_and_hold_on->value = true;
    menu->addChild(sample_and_hold_on);

    return menu;
  }

};
