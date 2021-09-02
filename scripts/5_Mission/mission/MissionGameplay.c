modded class MissionGameplay{
	
	void MissionGameplay() {	
		SUserConfigVisual userVisualCfg = SUserConfig.visual();
		SPPEManager.setDDOFBlurStrength(userVisualCfg.getDDOFIntensity());
		SPPEManager.setMotionBlurStrength(userVisualCfg.getMotionBlurIntensity());
		SPPEManager.setBloomStrength(userVisualCfg.getBloomIntensity());
	}
	
	override void OnUpdate(float timeslice){
		super.OnUpdate(timeslice);
		SPPEManager.onUpdate(timeslice);
	}
	
}