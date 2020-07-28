// Copyright 2001-2016 Crytek GmbH / Crytek Group. All rights reserved.

#include "StdAfx.h"
#include "GameGeomEntity.h"
#include "Game/GameFactory.h"

CGameGeomEntity::CGameGeomEntity()
{
}

CGameGeomEntity::~CGameGeomEntity()
{
}

bool CGameGeomEntity::Init(IGameObject *pGameObject)
{
	SetGameObject(pGameObject);
	return true;
}

void CGameGeomEntity::PostInit(IGameObject *pGameObject)
{
	Reset();
}

void CGameGeomEntity::Release()
{
	ISimpleExtension::Release();
}

bool CGameGeomEntity::RegisterProperties(SEntityScriptProperties &tables)
{
	if (tables.pPropertiesTable)
	{
		tables.pPropertiesTable->SetValue("object_Model", "editor/objects/sphere.cgf");
	}

	if (tables.pEditorTable)
	{
		tables.pEditorTable->SetValue("Icon", "prompt.bmp");
		tables.pEditorTable->SetValue("IconOnTop", true);
	}

	return true;
}

void CGameGeomEntity::ProcessEvent(SEntityEvent &event)
{
	switch (event.event)
	{
	case ENTITY_EVENT_EDITOR_PROPERTY_CHANGED:
	case ENTITY_EVENT_RESET:
	{
		Reset();
	}
	break;
	}
}

void CGameGeomEntity::Reset()
{
	SmartScriptTable propertiesTable;
	GetEntity()->GetScriptTable()->GetValue("Properties", propertiesTable);

	const char *geometryPath = "";
	if (propertiesTable->GetValue("object_Model", geometryPath))
	{
		GetEntity()->LoadGeometry(0, geometryPath);
	}
}

#include "FlowNodes/FlowBaseNode.h"

class CFlowGameEntityNode : public CFlowBaseNode<eNCT_Instanced>
{
	enum EFlowgraphInputPorts
	{
		eInputPorts_LoadGeometry,
	};

	enum EFlowgraphOutputPorts
	{
		eOutputPorts_Done,
	};

public:
	CFlowGameEntityNode(SActivationInfo *pActInfo)
	{
		m_lastInitializeFrameId = -1;
		m_entityId = GetEntityId(pActInfo);
	}

	virtual ~CFlowGameEntityNode()
	{
	}

	IFlowNodePtr CFlowGameEntityNode::Clone(SActivationInfo *pActivationInfo) override
	{
		return new CFlowGameEntityNode(pActivationInfo);
	}

	void CFlowGameEntityNode::GetConfiguration(SFlowNodeConfig &config) override
	{
		static const SInputPortConfig in_config[] = {
			InputPortConfig<string>("LoadGeometry", _HELP("Load static geometry")),
			{0}};
		static const SOutputPortConfig out_config[] = {
			OutputPortConfig<bool>("Done"),
			{0}};

		config.nFlags |= EFLN_TARGET_ENTITY | EFLN_HIDE_UI;
		config.SetCategory(EFLN_APPROVED);

		config.pInputPorts = in_config;
		config.pOutputPorts = out_config;
	}

	void CFlowGameEntityNode::ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo) override
	{
		FUNCTION_PROFILER(GetISystem(), PROFILE_ACTION);

		switch (event)
		{
		case eFE_SetEntityId:
		{
			m_entityId = GetEntityId(pActInfo);
		}
		break;
		case eFE_Activate:
		{
			if (IsPortActive(pActInfo, eInputPorts_LoadGeometry))
			{
				gEnv->pEntitySystem->GetEntity(m_entityId)->LoadGeometry(0, GetPortString(pActInfo, eInputPorts_LoadGeometry));			
				ActivateOutput(pActInfo, eOutputPorts_Done, TFlowInputData(true));
			}
		}
		break;
		case eFE_Initialize:
		{
			const int frameId = gEnv->pRenderer->GetFrameID(false);
			if (frameId != m_lastInitializeFrameId)
			{
				m_lastInitializeFrameId = frameId;
				m_entityId = GetEntityId(pActInfo);
				ActivateOutput(pActInfo, eOutputPorts_Done, SFlowSystemVoid());
			}
		}
		break;
		}
	}

	bool SerializeXML(SActivationInfo *pActInfo, const XmlNodeRef &, bool) override
	{
		return true;
	}

	virtual void GetMemoryUsage(ICrySizer *s) const override
	{
		s->Add(*this);
	}

protected:
	ILINE IEntity *GetEntity() const
	{
		return gEnv->pEntitySystem->GetEntity(m_entityId);
	}

	EntityId GetEntityId(SActivationInfo *pActInfo) const
	{
		assert(pActInfo);
		EntityId entityId = INVALID_ENTITYID;

		if (pActInfo && pActInfo->pEntity)
		{
			entityId = pActInfo->pEntity->GetId();
		}

		return entityId;
	}

	void Serialize(SActivationInfo *pActInfo, TSerialize ser) override
	{
		if (ser.IsReading())
		{
			m_lastInitializeFrameId = -1;
		}
	}

	int m_lastInitializeFrameId;
	EntityId m_entityId;
};

REGISTER_FLOW_NODE("entity:GameGeomEntity", CFlowGameEntityNode);