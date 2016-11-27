#ifndef TABLE_H
#define TABLE_H

// 台を定義するクラス
// TODO: DrawTable()の中身
// TODO: 台を構成する4つの壁、底の色と場所を定義する

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
