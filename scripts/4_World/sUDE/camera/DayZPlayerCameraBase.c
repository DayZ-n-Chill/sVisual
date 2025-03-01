modded class DayZPlayerCameraBase {
	
	protected DayZPlayerImplement m_iPlayer;
	protected static SUserConfigVisual userCfgVisual;
	
	protected static int m_ddofStartBoneIdx = -1;
	protected static ref SRaycast m_ddofRaycast = new SRaycast("0 0 0", "0 0 0", 0.05, ObjIntersectView, CollisionFlags.NEARESTCONTACT);
	
	protected float m_time;
	protected float m_headbobYawVel[1];
	protected float m_headbobPitchVel[1];
	
	
	void DayZPlayerCameraBase(DayZPlayer pPlayer, HumanInputController pInput) {
		userCfgVisual = SUserConfig.visual();
		m_iPlayer = DayZPlayerImplement.Cast(pPlayer);

		if (isDDOFEnabled() && userCfgVisual.getDDOFIntensity() != 0) {
			SPPEManager.enableDDOF();
		} else {
			SPPEManager.disableDDOF();
			SPPEManager.resetDDOF(true);
		}
		
		SCameraOverlaysManager.getInstance().setActiveCameraType(this.Type());
	}
	
	override void OnUpdate(float pDt, out DayZPlayerCameraResult pOutResult) {
		super.OnUpdate(pDt, pOutResult);
		updateDDOF(pDt, pOutResult);
		updateCamAngles(pDt, pOutResult);
	}
	
	protected void updateCamAngles(float pDt, DayZPlayerCameraResult pOutResult) {
		m_time += pDt;
		vector camAngles = Math3D.MatrixToAngles(pOutResult.m_CameraTM);
		
		if (isHeadbobEnabled()) {
			applyHeadBob(pDt, camAngles);
		}
		
		if (canApplyHeadLean()) {
			applyHeadLean(pDt, camAngles);
		}
		
		Math3D.YawPitchRollMatrix(camAngles, pOutResult.m_CameraTM);
	}
	
	protected bool canApplyHeadLean() {
		return isHeadLeanEnabled() && !m_pPlayer.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_RAISEDPRONE | DayZPlayerConstants.STANCEMASK_PRONE);
	}
	
	protected void applyHeadBob(float pDt, out vector angles) {
		TFloatArray headbobParams = getHeadbobParameters(); //@todo use player speed modifier		
		float yawStrenght = headbobParams[0];
		float yawFrequency = headbobParams[1];
		float pitchStrenght = headbobParams[2];
		float pitchFrequency = headbobParams[3];
		
		yawStrenght *= getHeadbobIntensity();
		pitchStrenght *= getHeadbobIntensity();
		
		//@todo smooth the transition using movSpeed
		angles[0] = Math.SmoothCD(angles[0], angles[0] + yawStrenght * Math.Sin(m_time * yawFrequency), m_headbobYawVel, 0.2, 1000, pDt);
		angles[1] = Math.SmoothCD(angles[1], angles[1] + pitchStrenght * Math.Sin(m_time * pitchFrequency), m_headbobPitchVel, 0.2, 1000, pDt); 
		
	}
	
	protected void applyHeadLean(float pDt, out vector angles) {
		angles[2] = angles[2] + getLeanRollAngle();
	}
	
	
	protected bool canRequestDDOF() {
		return isDDOFEnabled() && !m_pPlayer.IsCameraBlending());
	}
	
	protected void updateDDOF(float pDt, DayZPlayerCameraResult pOutResult) {
		if (canRequestDDOF()) {
			SPPEManager.requestDDOF(getFocusDistance());
		}
	}
	
	protected float getFocusDistance() {
		if (m_ddofStartBoneIdx == -1) {
			m_ddofStartBoneIdx = m_pPlayer.GetBoneIndexByName("Head");
			return 0;
		}
		
		vector from = m_pPlayer.GetBonePositionWS(m_ddofStartBoneIdx);
		
		m_ddofRaycast.init(
			from,
			from + (GetGame().GetCurrentCameraDirection() * SPPEManager.getDDOFMaxDistance()));
		
		m_ddofRaycast.ignore(m_pPlayer, m_pPlayer.GetDrivingVehicle());
		
		return vector.Distance(from, m_ddofRaycast.launch().getContactPosition());
	}
	
	
	protected TFloatArray getHeadbobParameters() {
		switch (m_iPlayer.m_MovementState.m_iMovement) { 
			
			case 0: // idling
			return HeadBobConstants.IDLE;

			case 1: // walking
			if (m_iPlayer.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_ERECT))       return HeadBobConstants.ERECT_WALKING;
			if (m_iPlayer.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_RAISEDERECT)) return HeadBobConstants.ERECT_RAISED_WALKING;
			if (m_iPlayer.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_CROUCH))      return HeadBobConstants.CROUCH_WALKING;
			if (m_iPlayer.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_PRONE))       return HeadBobConstants.PRONE_WALKING;
			break;
			
			case 2: // jogging
			if (m_iPlayer.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_ERECT))       return HeadBobConstants.ERECT_JOGGING;
			if (m_iPlayer.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_RAISEDERECT)) return HeadBobConstants.ERECT_RAISED_JOGGING;
			break;

			case 3: // running
			if (m_iPlayer.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_ERECT))       return HeadBobConstants.ERECT_RUNNING;
			if (m_iPlayer.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_CROUCH))      return HeadBobConstants.CROUCH_RUNNING;
			break;
			
		}
		return HeadBobConstants.IDLE;
	}
	
	protected bool isHeadbobEnabled() {
		return false;
	}
	
	protected bool isHeadLeanEnabled() {
		return false;
	}
	
	protected bool isDDOFEnabled() {
		return false;
	}
	
	protected float getLeanRollAngle() { //@todo report this. m_fLeaning doesn't reset when going prone while peeking (Q/E)
		return m_iPlayer.m_MovementState.m_fLeaning * getHeadLeanAngle();
	}
	
	protected static float getHeadbobIntensity() {
		return userCfgVisual.getHeadbobIntensity();
	}
	
	protected static float getHeadLeanAngle() {
		return userCfgVisual.getHeadLeanAngle();
	}
	
	protected static bool isDDOFEnabledInVehicle() {
		return userCfgVisual.isDDOFEnabledInVehicle();
	}
	
	protected static bool isHeadbobEnabledIn3pp() {
		return userCfgVisual.isHeadbobEnabledIn3pp();
	}
	
	protected static bool isDDOFEnabledIn3PP() {
		return userCfgVisual.isDDOFEnabledIn3PP();
	}
	
	
}