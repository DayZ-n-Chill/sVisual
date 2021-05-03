modded class DayZPlayerCamera3rdPerson {
	
	override void OnUpdate(float pDt, out DayZPlayerCameraResult pOutResult){
		super.OnUpdate(pDt, pOutResult);
		m_camManager.onUpdate(pDt, pOutResult);
		
		if( PPEManager.isPeakMitigationEnabled() ){
			updateDoF(pDt);
		}
	}
	
	override bool isHeadbobEnabled(){
		return false;
	}
	
	override bool isHeadLeanEnabled(){
		return false;
	}
	
}