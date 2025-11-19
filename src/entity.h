#ifndef ENTITY_H
#define ENTITY_H

// Set up a class for Entity
class Entity {
public:
	int x;
	int y;
	bool isGrabed = false;
	bool isAlive = true;
	bool isPassable = false;

	void SetIndex(int x, int y);
	void SetX(int newx) { newx = index_x; }
	void SetY(int newy) { newy = index_y; }
	int IndexX() const { return index_x; }
	int IndexY() const { return index_y; }
private:
	int index_x;
	int index_y;
};


#endif // ENTITY_H
