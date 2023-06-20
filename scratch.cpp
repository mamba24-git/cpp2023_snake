#define __SNAKE_H__

#include <vector>
#include <string>
#include <iostream>
using namespace std;

class Object
{
protected:
	// 화면에 보이게 될 기호 문자(유니코드이므로 string으로 지정함)
	string symbol;

public:
	Object()
	{
		symbol = "?";
	}

	string getSymbol()
	{
			return symbol;
	}
};

class Empty : public Object
{
public:
	Empty()
	{
		symbol = " ";
	}
};

class Wall : public Object
{
public:
	Wall()
	{
		symbol = "#";
	}
};

class ImmuneWall : public Wall
{
public:
	ImmuneWall()
	{
		symbol = "E";
	}
};

class Gate : public Wall
{
public:
	Gate()
	{
		symbol = "X";
	}
};

class Item : public Object
{
protected:
	// 아이템이 생성된 절대 초 -> 아이템이 사라지는 때를 계산하기 위한 것
	time_t CREATED_TIME;

public:
	Item()
	{
		symbol = "*";
		CREATED_TIME = time(NULL);
	}
};

class Growth : public Item
{
public:
	Growth()
	{
		symbol = "+";
	}
	time_t time_check()
	{
		return CREATED_TIME;
	}
};

class Poison : public Item
{
public:
	Poison()
	{
		symbol = "-";
	}
	time_t time_check()
	{
		return CREATED_TIME;
	}
};
class Manager;
class Item;

/* 좌표를 직관적으로 표시하기 위한 클래스 */
class Pos
{
public:
  int x;
  int y;

public:
  Pos() {}

  Pos(int x, int y) : x(x), y(y) {}

  //=연산자
  Pos &operator=(const Pos &s)
  {
    this->x = s.x;
    this->y = s.y;
    return *this;
  }

  friend bool operator==(const Pos &p1, const Pos &p2)
  {
    return (p1.x == p2.x) && (p1.y == p2.y);
  }

  friend bool operator!=(const Pos &p1, const Pos &p2)
  {
    return !(p1 == p2);
  }

  friend ostream &operator<<(ostream &os, const Pos &p)
  {
    cout << "(" << p.x << ", " << p.y << ")";
    return os;
  }
};

/* 방향을 표시하기 위한 클래스. 좌표와 심볼로 표현할 수 있음. */
class Direction
{
private:
  int x;
  int y;

public:
  Direction() : x(0), y(0) {}

  Direction(int x, int y) : x(x), y(y) {}

  // char형 매개 변수(이하 symbol)를 받아 Direction 객체를 만들어 반환
  static Direction getDirectionBySymbol(char symbol)
  {
    switch (symbol)
    {
    case 'L':
      return Direction(-1, 0);
    case 'R':
      return Direction(1, 0);
    case 'U':
      return Direction(0, -1);
    case 'D':
      return Direction(0, 1);
    }

    cout << "ERROR: Invalid Direction Symbol '" << symbol << "'\n";
    return Direction(0, 0);
  }

  // 전달된 symbol의 반대 방향 Direction 객체를 만들어 반환
  static Direction getOppositeDirection(char symbol)
  {
    switch (symbol)
    {
    case 'L':
      return Direction::getDirectionBySymbol('R');
    case 'R':
      return Direction::getDirectionBySymbol('L');
    case 'U':
      return Direction::getDirectionBySymbol('D');
    case 'D':
      return Direction::getDirectionBySymbol('U');
    }

    cout << "ERROR: On getOppositeDirection, Invalid Direction Symbol '" << symbol << "'\n";
    return Direction(0, 0);
  }

  // 해당 객체의 방향을 symbol로 표현해 반환
  char getSymbol()
  {
    // 왼쪽
    if (x == -1 && y == 0)
      return 'L';
    // 오른쪽
    else if (x == 1 && y == 0)
      return 'R';
    // 위쪽
    else if (x == 0 && y == -1)
      return 'U';
    // 아래쪽
    else if (x == 0 && y == 1)
      return 'D';
    // 알 수 없는 경우
    else
      return 'X';
  }

  //해당 객체의 방향을 변경
  void setDirection(int x, int y)
  {
    this->x = x;
    this->y = y;
  }

  // 해당 객체의 좌표 성분을 반환
  int getXDirection() { return x; }
  int getYDirection() { return y; }

  Direction &operator=(const Direction &s)
  {
    this->x = s.x;
    this->y = s.y;
    return *this;
  }

  bool operator==(const Direction &d)
  {
    return (this->x == d.x) && (this->y == d.y);
  }

  bool operator!=(const Direction &d)
  {
    return !(operator==(d));
  }

  // 해당 객체의 시계 방향으로 회전하는 방향을 반환
  Direction getClockwise();

  // 해당 객체의 반시계방향으로 회전하는 방향을 반환
  Direction getCounterClockwise();

  // 전달된 방향이 메서드 호출 방향과 정반대라면 true, 아니라면 false를 반환
  bool isOppositeWith(Direction &d);
};

/* Snake를 구성하는 몸통(+머리) */
class Body
{
private:
  // Body의 현재 위치
  Pos currentPos;
  // Body가 다음으로 이동할 좌표
  Pos nextPos;

public:
  Body() {}

  // 항상 최초 위치 바로 이후의 스케줄까지 입력받아야 한다.
  Body(int initX, int initY, int nextX, int nextY)
  {
    currentPos = Pos(initX, initY);
    nextPos = Pos(nextX, nextY);
  }

  Pos getPos()
  {
    return currentPos;
  }

  // Body의 현재 위치를 다음 위치로 갱신한다.
  void updateCurrentPos()
  {
    currentPos = nextPos;
  }

  //snake head 방향에 따라 snake head의 이동 스케줄을 새롭게 추가한다.
  void updateHeadSchedule(char symbol);

  //snake 몸통의 현재 좌표를 반환
  int getCurrentX() { return currentPos.x; }
  int getCurrentY() { return currentPos.y; }

  // 현재 좌표 정보 갱신
  void setPos(Pos p)
  {
    currentPos = Pos(p.x, p.y);
  }

  // 다음 위치를 해당 좌표로 바꿔줌
  void setNextPos(Pos p)
  {
    nextPos = p;
  }

  // 다음 위치를 반환
  Pos getNextPos()
  {
    return nextPos;
  }
};

/* Snake의 상태를 저장/갱신하기 위한 클래스 */
class Snake
{
private:
  const static int MIN_LENGTH = 3; // 뱀의 최소 몸길이
  int length;                      // 뱀의 몸 길이
  int growthCnt;                   // Growth 획득 횟수
  int poisonCnt;                   // Posion 획득 횟수
  int gateCnt;                     // Gate 이용 횟수
  Direction lastDirection;         // Snake의 마지막 이동 방향
  vector<Body> bodies;             // Snake의 머리와 몸통들을 요소로 하는 벡터

  // *= 연산자 오버로딩에 사용됨)
  int initX, initY;

public:
  // Constructor
  Snake() {}
  Snake(int initX, int initY);
  Snake(Pos initPos);

  Snake &operator=(const Snake &s);

  // Getter ============================================
  // 몸 길이를 반환
  int getLength();
  // 획득한 growth의 개수를 반환
  int getGrowthCnt();
  // 획득한 poison의 개수를 반환
  int getPoisonCnt();
  // 게이트 통과 횟수를 반환
  int getGateCnt();
  // body 객체를 담은 vector를 반환
  vector<Body> getBodies();
  // 입력된 마지막 입력 방향을 반환
  Direction getlastdirection();

  /* body의 맨 앞 레퍼런스(머리)를 반환
     Precondition: bodies가 비어 있으면 안됨 */
  Body *getHead();
  // head의 좌표를 Pos로 반환
  Pos getHeadPos();
  // 맨 마지막 Body의 레퍼런스를 반환
  Body getLastBody();

  // Setter =============================================
  // 획득한 growth의 개수를 갱신
  void setGrowhCnt(int cnt);
  // 획득한 poison의 개수를 갱신
  void setPoisonCnt(int cnt);
  // 게이트 통과 횟수를 갱신
  void setGateCnt(int cnt);
  // snake의 머리 위치를 갱신
  void setHeadPos(Pos p);
  // 마지막으로 입력된 방향을 갱신
  void setLastDirection(Direction d);

  // 몸 길이 관련 ========================================
  /* Snake의 길이를 1만큼 줄이는 함수
    길이를 더 줄일 수 없다면(줄이면 뱀이 죽는다면) 길이를 갱신하지 않고 false를 반환
    길이를 더 줄일 수 있다면 길이와 Body 정보를 갱신하고 true를 반환 */
  bool shorten();

  // Snake의 길이를 1만큼 늘이고 Body 정보를 갱신하는 함수
  void lengthen();

  // 아이템 관련 =========================================
  // 게이트를 사용해 Body의 위치와 이동 경로를 수정함.일관성을 위해 항상 true를 반환하도록 설정 bool
  bool useGate(Manager &m, Direction direction);

  // 이동 관련 ============================================
  /* newDirection 방향으로 Snake를 이동시키고, lastDirection을 갱신함
    만약 newDirection이 lastDirection과 반대 방향이라면, false를 반환해 게임을 종료할 수 있도록 하고
    그렇지 않은 경우 true를 반환하여 메서드가 제대로 실행 되었음을 외부에 알림
    단, 아이템이나 벽 충돌, 몸통 충돌 등으로 인한 게임 종료는 함수 외부에서 판단함 */
  bool changeHeadDirection(Direction newDirection);

  //snake의 몸통 이동 데이터 갱신
  void moveTo();

  // 상태 점검 =============================================
  /* 머리가 자신의 몸통과 부딪혔는지 확인하는 메서드
    Precondition: moveTo 메서드에 의해 이동이 완전히 끝난 후에 호출해야 함*/
  bool isBumpedToBody();
};

// Direction begins =============================================
// 해당 객체의 시계 방향으로 회전하는 방향을 반환
Direction Direction::getClockwise()
{
  switch (getSymbol())
  {
  // 오른쪽 -> 아래
  case 'R':
    return Direction::getDirectionBySymbol('D');
  // 아래 -> 왼쪽
  case 'D':
    return Direction::getDirectionBySymbol('L');
  // 왼쪽 -> 위쪽
  case 'L':
    return Direction::getDirectionBySymbol('U');
  // 위쪽 -> 오른쪽
  case 'U':
    return Direction::getDirectionBySymbol('R');
  default:
    cout << "ERROR: getClockwise() got invalid direction" << endl;
    return Direction(0, 0);
  }
}

// 해당 객체의 반시계방향으로 회전하는 방향을 반환
Direction Direction::getCounterClockwise()
{
  switch (getSymbol())
  {
  // 오른쪽 -> 위
  case 'R':
    return Direction::getDirectionBySymbol('U');
  // 위 -> 왼쪽
  case 'U':
    return Direction::getDirectionBySymbol('L');
  // 왼쪽 -> 아래쪽
  case 'L':
    return Direction::getDirectionBySymbol('D');
  // 아래쪽 -> 오른쪽
  case 'D':
    return Direction::getDirectionBySymbol('R');
  default:
    cout << "ERROR: getCounterClockwise() got invalid direction" << endl;
    return Direction(0, 0);
  }
}

// 전달된 방향이 메서드 호출 방향과 정반대라면 true, 아니라면 false를 반환
bool Direction::isOppositeWith(Direction &d)
{
  return Direction::getOppositeDirection(d.getSymbol()) == *this;
}
// Direction finished ===========================================

// Body begins ==================================================
void Body::updateHeadSchedule(char symbol)
{
  if (symbol == 'L')
    nextPos = Pos(currentPos.x - 1, currentPos.y);
  else if (symbol == 'R')
    nextPos = Pos(currentPos.x + 1, currentPos.y);
  else if (symbol == 'U')
    nextPos = Pos(currentPos.x, currentPos.y - 1);
  else if (symbol == 'D')
    nextPos = Pos(currentPos.x, currentPos.y + 1);
}
// Body finished ================================================

// Snake begins =================================================
Snake::Snake(int initX, int initY)
{
  // ()= 연산자 오버로딩에 사용됨)
  this->initX = initX;
  this->initY = initY;

  // 몸 길이 초기화
  length = 3;
  // 아이템 획득/게이트 통과 횟수 초기화
  growthCnt = 0;
  poisonCnt = 0;
  gateCnt = 0;

  // 왼쪽 방향을 기본 방향으로 함
  lastDirection = Direction::getDirectionBySymbol('L');

  // 맨 처음에는 ">~~" 이렇게 왼쪽을 보고 움직이도록 설정
  bodies.push_back(Body(initX, initY, initX - 1, initY));
  bodies.push_back(Body(initX + 1, initY, initX, initY));
  bodies.push_back(Body(initX + 2, initY, initX + 1, initY));
}

Snake::Snake(Pos initPos)
{
  // ()= 연산자 오버로딩에 사용됨)
  this->initX = initPos.x;
  this->initY = initPos.y;

  // 몸 길이 초기화
  length = 3;
  // 아이템 획득/게이트 통과 횟수 초기화
  growthCnt = 0;
  poisonCnt = 0;
  gateCnt = 0;

  // 왼쪽 방향을 기본 방향으로 함
  lastDirection = Direction::getDirectionBySymbol('L');

  // 맨 처음에는 ">~~" 이렇게 왼쪽을 보고 움직이도록 설정
  bodies.push_back(Body(initX, initY, initX - 1, initY));
  bodies.push_back(Body(initX + 1, initY, initX, initY));
  bodies.push_back(Body(initX + 2, initY, initX + 1, initY));
}

Snake &Snake::operator=(const Snake &s)
{
  // ()= 연산자 오버로딩에 사용됨)
  this->initX = s.initX;
  this->initY = s.initY;

  // 몸 길이 초기화
  length = 3;
  // 아이템 획득/게이트 통과 횟수 초기화
  growthCnt = 0;
  poisonCnt = 0;
  gateCnt = 0;

  // 왼쪽 방향을 기본 방향으로 함
  lastDirection = Direction::getDirectionBySymbol('L');

  // 맨 처음에는 "@00" 이렇게 왼쪽을 보고 움직이도록 설정
  bodies.push_back(Body(initX, initY, initX - 1, initY));
  bodies.push_back(Body(initX + 1, initY, initX, initY));
  bodies.push_back(Body(initX + 2, initY, initX + 1, initY));

  return *this;
}

// Getter ============================================
int Snake::getLength() { return length; }
int Snake::getGrowthCnt() { return growthCnt; }
int Snake::getPoisonCnt() { return poisonCnt; }
int Snake::getGateCnt() { return gateCnt; }
vector<Body> Snake::getBodies() { return bodies; }
Direction Snake::getlastdirection() { return lastDirection; }

Body *Snake::getHead()
{
  return &bodies[0];
}
Pos Snake::getHeadPos()
{
  return bodies[0].getPos();
}
Body Snake::getLastBody()
{
  return bodies[length - 1];
}

// Setter =============================================
void Snake::setGrowhCnt(int cnt) { this->growthCnt = cnt; };
void Snake::setPoisonCnt(int cnt) { this->poisonCnt = cnt; };
void Snake::setGateCnt(int cnt) { this->gateCnt = cnt; };
void Snake::setHeadPos(Pos p)
{
  bodies[0].setPos(p);
}
void Snake::setLastDirection(Direction d)
{
  lastDirection = d;
}

// 몸 길이 관련 ========================================
//snake의 몸 길이를 1만큼 줄이기
bool Snake::shorten()
{
  if (length == MIN_LENGTH) //현재 몸 길이가 3인 경우 줄일 수가 없기 때문에 false 반환
    return false;
  else //현재 몸 길이가 3보다 큰 경우
  {
    length -= 1;
    bodies.pop_back();
    return true;
  }
}
/*snake의 몸 길이를 1만큼 늘리기
  snake의 lastDirection의 값에 따라 새로운 body객체 생성 및 bodies 벡터에 추가
  몸 길이를 나타내는 변수에 -1*/
void Snake::lengthen()
{
  char symbol = this->lastDirection.getSymbol();
  if (symbol == 'L')
    bodies.push_back(Body(bodies[length - 1].getCurrentX() + 1, bodies[length - 1].getCurrentY(), bodies[length - 1].getCurrentX(), bodies[length - 1].getCurrentY()));
  if (symbol == 'R')
    bodies.push_back(Body(bodies[length - 1].getCurrentX() - 1, bodies[length - 1].getCurrentY(), bodies[length - 1].getCurrentX(), bodies[length - 1].getCurrentY()));
  if (symbol == 'U')
    bodies.push_back(Body(bodies[length - 1].getCurrentX(), bodies[length - 1].getCurrentY() + 1, bodies[length - 1].getCurrentX(), bodies[length - 1].getCurrentY()));
  if (symbol == 'D')
    bodies.push_back(Body(bodies[length - 1].getCurrentX(), bodies[length - 1].getCurrentY() - 1, bodies[length - 1].getCurrentX(), bodies[length - 1].getCurrentY()));

  length += 1;
}

// 이동 관련 ============================================
bool Snake::changeHeadDirection(Direction newDirection)
{
    char symbol = this->lastDirection.getSymbol();

    // 새 방향이 기존 방향과 정반대인 경우 게임 오버
    if (newDirection.isOppositeWith(lastDirection))
        return false;
    // 새로 입력된 방향이 유효한 경우
    else if (newDirection.getSymbol() != 'X')
    {
        this->lastDirection = newDirection;
        return true;
    }
    // 알 수 없는 심볼인 경우
    else
        return false;
}

void Snake::moveTo()
{
    // 마지막으로 입력된 방향의 심볼
    char symbol = this->lastDirection.getSymbol();

    // 머리 위치 갱신
    bodies[0].updateCurrentPos();
    bodies[0].updateHeadSchedule(symbol);

    // 몸통 위치 갱신
    for (int i = 1; i < bodies.size(); i++)
    {
        bodies[i].updateCurrentPos();
        bodies[i].setNextPos(Pos(bodies[i - 1].getCurrentX(), bodies[i - 1].getCurrentY()));
    }
}

// 상태 점검 =============================================
bool Snake::isBumpedToBody()
{
    for (int i = 1; i < bodies.size(); i++)
    {
        if ((bodies[0].getCurrentX() == bodies[i].getCurrentX()) && (bodies[0].getCurrentY() == bodies[i].getCurrentY()))
        {
            return true;
        }
    }
    return false;
}
// Snake finished ===============================================