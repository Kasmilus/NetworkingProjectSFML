#pragma once
#include "Input.h"

/*
Class used to sample player input for each client on the server
Clients use singleton Input class
*/

class ClientCommandInput :
	public Input
{
public:
	ClientCommandInput();
	~ClientCommandInput();

	virtual void Update() override;

	void UpdateInput(float horInput, float verInput, bool actionButtonDown);
	inline void UpdateTauntInput(bool value) { tauntButtonDown = value; }		// NOT USED ANYWHERE YET

	bool leftPressed, rightPressed, upPressed, downPressed;

};

