modded class PlayerBase{
	
	protected ref PPETimedParams ppeSpawn = new PPETimedParams(5, true);                       //Used when spawning	
	protected ref PPEHitReceivedAnimation ppeHitAnim = new PPEHitReceivedAnimation(4.5, true); //Used when being hit
	protected ref PPEEyegearPreset ppeEye = new PPEEyegearPreset();                            //Used when wearing AviatorGlasses
	protected ref PPEBleedingAnimation ppeBleeding = new PPEBleedingAnimation();               //Used when bleeding
	protected ref PPEUnconsciousAnimation m_ppeUnconscious = new PPEUnconsciousAnimation();    //Used when going uncoscious
	
	//Debug
	protected ref PPEAnimatedParams ppeDebug = new PPEDebugAnimation();
	
	override void OnInventoryMenuOpen(){
		super.OnInventoryMenuOpen();
		//PPEManager.activate(ppeDebug);	
	}
	
	override void OnInventoryMenuClose(){
		super.OnInventoryMenuClose();
		//PPEManager.deactivate(ppeDebug);	
	}
	
	override void EEItemAttached(EntityAI item, string slot_name){
		super.EEItemAttached(item, slot_name);
		
		//@todo quick fix for main menu eyewear
		// Apply colored overlay when wearing AviatorGlasses
		if( GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_CLIENT && slot_name == "Eyewear" && AviatorGlasses.Cast(item)){
			PPEManager.activate(ppeEye);
		}
	}
	
	override void EEItemDetached(EntityAI item, string slot_name){
		super.EEItemDetached(item, slot_name);
		
		// Remove colored overlay when removing AviatorGlasses
		if( slot_name == "Eyewear" && AviatorGlasses.Cast(item)){		
			PPEManager.deactivate(ppeEye);
		}
	}
	
	//@todo fix this, EEHitBy gets called only on server
	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef){
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
				
		// Apply hit effect based on the damage received
		float dmgReceived = damageResult.GetHighestDamage("Health");
		ppeHitAnim.setDuration(dmgReceived * 0.25);
		ppeHitAnim.setHitStrength(dmgReceived);
		
		PPEManager.activate(ppeHitAnim);
		
	}
	
	override void OnJumpStart(){		
		super.OnJumpStart();
	}
			
	//@todo don't use on select player
	override void OnSelectPlayer(){
		super.OnSelectPlayer();
		
		//Remove all ppeffects
		PPEManager.deactivateAll();
		PPEManager.applyDefault();
		
		//Proceed to apply effect only if controlled player 
		if( GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_CLIENT ){
			
			//@todo complete activation/deactivation on connection/disconnection	
			playSpawnPPE();		
			checkForGlassesPPE();
			checkForBleedingPPE();		
		}
	}
	
	protected void playSpawnPPE(){
		ppeSpawn.setVignette(20, PPEManager.getPPEColor(0,0,0,0));
		PPEManager.activate(ppeSpawn);
	}
	
	protected void checkForBleedingPPE(){
		if(IsBleeding()){
			updateBleedingEffect();
			PPEManager.activate(ppeBleeding);
		}
	}
	
	protected void checkForGlassesPPE(){
		if ( AviatorGlasses.Cast( GetInventory().FindAttachment(InventorySlots.EYEWEAR)) ){
			PPEManager.activate(ppeEye);
		}
	}	
	
	
	
	
	///////////////// UNCONSCIOUSNESS ///////////////////////////////
	override void OnUnconsciousStart(){
		super.OnUnconsciousStart();
		PPEManager.activate(m_ppeUnconscious);
	}
	
	override void OnUnconsciousStop(int pCurrentCommandID){
		super.OnUnconsciousStop(pCurrentCommandID);
		PPEManager.deactivate(m_ppeUnconscious);
	}
	
	
	///////////////// BLEEDING ///////////////////////////////
	override void OnBleedingBegin(){
		super.OnBleedingBegin();
		PPEManager.activate(ppeBleeding);
	}
	
	override void OnBleedingEnd(){
		super.OnBleedingEnd();
		PPEManager.deactivate(ppeBleeding);
	}
	
	override void OnBleedingSourceAdded(){
		super.OnBleedingSourceAdded();
		updateBleedingEffect();
		
	}
	
	override void OnBleedingSourceRemoved(){
		super.OnBleedingSourceRemoved();
		updateBleedingEffect();
	}
	
	protected void updateBleedingEffect(){
		ppeBleeding.setBleedingBits(GetBleedingSourceCount());
	}
	
}
