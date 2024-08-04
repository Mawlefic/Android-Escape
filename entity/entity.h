#pragma once

class Entity {

public:
	virtual void Draw() = 0;
	virtual void Update(float delta) = 0;
};