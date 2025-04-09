#include "plugin.hpp"
#include "vgLib-2.0/constants.cpp"

#if !defined(METAMODULE_BUILTIN)
Plugin* pluginInstance;
#else
extern Plugin* pluginInstance;
#endif

#if !defined(METAMODULE_BUILTIN)
void init(Plugin* p)
#else
void init_VoxGlitch(Plugin* p)
#endif
{
  pluginInstance = p;
  // p->addModel(modelArpSeq);
  p->addModel(modelAutobreak);
  // p->addModel(modelAutobreakStudio);  
  p->addModel(modelByteBeat);
  // p->addModel(modelDigitalProgrammer);
  // p->addModel(modelDigitalSequencer);
  // p->addModel(modelDigitalSequencerXP);
  p->addModel(modelDrumRandomizer);
  // p->addModel(modelCueResearch); 
  p->addModel(modelGlitchSequencer);
  p->addModel(modelGhosts);
  // p->addModel(modelGrainEngineMK2);
  // p->addModel(modelGrainEngineMK2Expander);
  // p->addModel(modelGrainFx);
  // p->addModel(modelGrooveBox);
  // p->addModel(modelGrooveBoxExpander);
  // p->addModel(modelHazumi);
  // p->addModel(modelOnePoint);
  // p->addModel(modelOneZero);
  // p->addModel(modelLooper);
  // p->addModel(modelNoteDetector);
  // p->addModel(modelRepeater);
  // p->addModel(modelSamplerX8);
  // p->addModel(modelSampler16P);
  // p->addModel(modelSatanonaut);
  // p->addModel(modelSatanonautUnearthed);
  // p->addModel(modelVectorRotation);
  // p->addModel(modelWavBank);
  // p->addModel(modelWavBankMC);
  // p->addModel(modelXY);
}
