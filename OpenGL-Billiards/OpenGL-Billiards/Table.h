#ifndef TABLE_H
#define TABLE_H

// ����`����N���X
// TODO: DrawTable()�̒��g
// TODO: ����\������4�̕ǁA��̐F�Əꏊ���`����

#include "vec3.h"

class Table {
public:
	vec3 position;

	Table(){}
	Table(vec3 pos){}
	void DrawTable() {}
	void setPos(vec3 pos) {}

private:

};

Table::Table() {}

Table::Table(vec3 pos){
	position = pos;
}

void Table::DrawTable() {

}

void Table::setPos(vec3 pos) {
	position = pos;
}


#endif
