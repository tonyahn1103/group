#include <bangtal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ANIMATION_TIME		0.01f
#define ATTACK_TIME			2.0f
#define ATTACK_SHOWN_TIME	0.05f
#define SKILL1_TIME			5.0f
#define SKILL1_STEP			100
#define SKILL2_TIME			10.0f
#define SKILL3_TIME			10.0f
#define BARRIER_TIME		3.0f
#define CHECK_TIME			0.01f
#define CRITICAL_TIME       5.0f

SceneID start, store, gamescene, gameoverscene;
ObjectID startbutton, attack, speed, attackspeed, hp, critical, move, stun, shield, stp1, stp2, gamestartbutton;
TimerID lobby_timer1, lobby_timer2;

int att = 50;
int spe = 15;
float attack_time = ATTACK_TIME;
int cri = 0;
int m_att = 101;
bool skill1 = false, skill2 = false, skill3 = false;

ObjectID player, p_attack, p_half_attack, skillButton1, skillButton2, skillButton3, HP, cooldown1, cooldown2, cooldown3, p_shield, m_bubble, b1, b2, b3, b4;
TimerID player_animation, attack_animation0, attack_animation1, attack_animation2, skill1_timer, skill2_timer, skill3_timer1, skill3_timer2, skill4_timer, skill2_timer2, barrier_timer;
KeyCode c;

TimerID check_timer;
bool damaged = false;

int dx = 0, playerX = 600, playerY = 0, attackX = 0, attackY = 180;

ObjectID m_HP_bar, m_HP_bar_back, p_HP_bar, p_HP_bar_back;
int m_HP_barX = 0, m_HP_barY = 680, p_HP_barX = 0, p_HP_barY = 0;

bool doll_m_check = false, doll_p_check = false, barrier_shown = false;

ObjectID m, warning, beam, half_beam, hill;
TimerID m_timer1, m_timer2, m_timer3, m_timer4;
float m_movetime = 4.f, m_warning = 0.5f, m_beam = 0.5f;
int mx;

int t = 0;

ObjectID doll;
int dollX = 1;
int dollSpeed = 5;
TimerID d_timer1, d_timer2;

//after
TimerID after_timer;

bool beamshown = false, attackshown = false, cr = false;

void critic()
{
	int num;
	srand(time(0));
	num = rand() % 100;
	if (cri > num) {
		att = att * 2;
		cr = true;
	}
}

void lobby_timerCallback(TimerID timer)
{
	if (timer == lobby_timer1) {
		setTimer(lobby_timer1, 0.2f);
		showObject(stp2);
		hideObject(stp1);
		startTimer(lobby_timer2);
	}
	if (timer == lobby_timer2) {
		setTimer(lobby_timer2, 0.2f);
		showObject(stp1);
		hideObject(stp2);
		startTimer(lobby_timer1);
	}
}

void skills()
{
	if (skill1 == true) {
		showObject(skillButton1);
	}
	if (skill2 == true) {
		showObject(skillButton2);
	}
	if (skill3 == true) {
		showObject(skillButton3);
	}
}

//게임스테이지에서 로비로 넘어갈때
void endgamescene()
{
	hideTimer();

	stopTimer(m_timer1);
	stopTimer(m_timer2);
	stopTimer(m_timer3);
	stopTimer(m_timer4);
	stopTimer(d_timer1);
	stopTimer(d_timer2);
	stopTimer(player_animation);
	stopTimer(attack_animation0);
	stopTimer(attack_animation1);
	stopTimer(attack_animation2);
	stopTimer(skill1_timer);
	stopTimer(skill2_timer);
	stopTimer(skill3_timer1);
	stopTimer(skill3_timer2);
	stopTimer(skill4_timer);
	stopTimer(check_timer);

	//물풍선에 있는채로 죽었을때
	hideObject(m_bubble);
	showObject(m);
	hideObject(cooldown2);

	//쉴드 죽었을때
	hideObject(p_shield);
	hideObject(cooldown3);
	damaged = false;


	//순간이동
	hideObject(cooldown1);

	hideObject(beam);
	hideObject(half_beam);
	hideObject(warning);

	p_HP_barX = 0;//플레이어 몬스터 체력 초기화
	m_HP_barX = 0;
	locateObject(m_HP_bar, gamescene, m_HP_barX, m_HP_barY);
	locateObject(p_HP_bar, gamescene, p_HP_barX, p_HP_barY);
	m_att += 20;					//몬스터 공격력 증가
	m_movetime = m_movetime * 0.8;				//몬스터 이동시간 감소
	m_warning = m_warning * 0.8;	
	m_beam = m_beam * 0.8;			

	showObject(skill1);
	showObject(skill2);
	showObject(skill3);

	enterScene(store);
}

void gameover()
{
	stopTimer(m_timer1);
	stopTimer(m_timer2);
	stopTimer(m_timer3);
	stopTimer(m_timer4);
	stopTimer(d_timer1);
	stopTimer(d_timer2);
	stopTimer(player_animation);
	stopTimer(attack_animation1);
	stopTimer(attack_animation2);
	stopTimer(skill1_timer);
	stopTimer(skill2_timer);
	stopTimer(skill3_timer1);
	stopTimer(skill3_timer2);
	stopTimer(skill4_timer);
	stopTimer(check_timer);

	showMessage("게임 오버");
	enterScene(start);

	showObject(move);
	showObject(stun);
	showObject(shield);
	hideObject(skillButton1);
	hideObject(skillButton2);
	hideObject(skillButton3);

	hideObject(beam);

	skill1 = false;
	skill2 = false;
	skill3 = false;
	showObject(m);
	p_HP_barX = 0;
	m_HP_barX = 0;
	locateObject(m_HP_bar, gamescene, m_HP_barX, m_HP_barY);
	locateObject(p_HP_bar, gamescene, p_HP_barX, p_HP_barY);
	att = 50;
	m_att = 101;
	m_movetime = 4.0f;
	m_warning = 0.5f;
	m_beam = 0.5f;
}

void damage_check()
{
	if (attackshown == true) {
			if (attackX > mx - 10 && attackX < mx + 210) {
				m_HP_barX -= att;
				locateObject(m_HP_bar, gamescene, m_HP_barX, m_HP_barY);

				if (m_HP_barX < -300) {
					endgamescene();
				}
			}
		}
	if (beamshown == true) {
		if (damaged == false) {
			if (mx > playerX - 215 && mx < playerX + 45) {
				p_HP_barX -= m_att;
				locateObject(p_HP_bar, gamescene, p_HP_barX, p_HP_barY);

				if (p_HP_barX < -300) {
					gameover();
				}
			}
		}
	}
}

void doll_check()
{
	if (attackX > dollX - 10  && attackX < dollX + 200) {
		doll_p_check = true;
	}
	if (mx > dollX - 10 && mx < dollX + 200) {
		doll_m_check = true;
	}
}

void monster_move()
{
	mx = rand() % 1121;

	locateObject(m, gamescene, mx, 330);
	showObject(m);
	locateObject(m_bubble, gamescene, mx, 370);


	setTimer(m_timer1, m_movetime);
	startTimer(m_timer1);
}

void monster_timerCallback(TimerID timer)
{
	if (timer == m_timer1) {
		showObject(m);
		monster_move();
		setTimer(m_timer1, m_movetime);
		startTimer(m_timer1);

		setTimer(m_timer2, m_warning);
		startTimer(m_timer2);
	}
	if (timer == m_timer2) {
		locateObject(warning, gamescene, mx, 0);
		showObject(warning);
		doll_m_check = false;

		setTimer(m_timer3, m_beam);
		startTimer(m_timer3);
	}
	if (timer == m_timer3) {
		hideObject(warning);

		doll_check();

		if (doll_m_check == true) {
			locateObject(half_beam, gamescene, mx, 335);
			showObject(half_beam);
			doll_m_check = false;
		}
		else {
			locateObject(beam, gamescene, mx, 0);
			showObject(beam);
			beamshown = true;
			damage_check();
		}

		setTimer(m_timer4, 0.1f);
		startTimer(m_timer4);
	}
	if (timer == m_timer4) {
		hideObject(beam);
		beamshown = false;
		hideObject(half_beam);
	}

}

void doll_timerCallback(TimerID timer)
{
	if (timer == d_timer1) {
		if (dollX < 1100) {
			dollX += dollSpeed;
			locateObject(doll, gamescene, dollX, 255);
			setTimer(d_timer1, 0.01f);
			startTimer(d_timer1);

		}

		if (dollX >= 1100) {

			stopTimer(d_timer1);
			startTimer(d_timer2);
		}
	}

	if (timer == d_timer2) {
		dollX -= dollSpeed;
		locateObject(doll, gamescene, dollX, 255);
		setTimer(timer, 0.01f);
		startTimer(d_timer2);

		if (dollX < 10) {
			stopTimer(d_timer2);
			startTimer(d_timer1);
		}

	}
}

//게임 화면으로 넘어가는 함수
void startgamescene()
{
	locateObject(m_HP_bar, gamescene, m_HP_barX, m_HP_barY);
	locateObject(p_HP_bar, gamescene, p_HP_barX, p_HP_barY);
	showObject(m_HP_bar);
	showObject(m_HP_bar);

	showTimer(attack_animation1);

	t = 0;
	enterScene(gamescene);
	locateObject(m, gamescene, mx, 330);
	locateObject(m_bubble, gamescene, mx, 370);

	setTimer(m_timer1, m_movetime);
	startTimer(m_timer1);

	setTimer(d_timer1, 0.01f);
	startTimer(d_timer1);

	setTimer(player_animation, ANIMATION_TIME);
	startTimer(player_animation);

	setTimer(attack_animation0, 2.f);
	startTimer(attack_animation0);

	setTimer(check_timer, CHECK_TIME);
	startTimer(check_timer);

	skills();
}

void lobby_mouseCallback(ObjectID object, int x, int y, MouseAction action)
{
	if (object == attack) {
		if (t == 0) {
			att += 50;
			showMessage("공격력 증가!");
			t = 1;
		}
		else {
			showMessage("더 살 수 없습니다.");
		}
	}
	else if (object == startbutton) {
		startTimer(lobby_timer1);
		enterScene(store);
	}
	else if (object == speed) {
		if (t == 0) {
			spe += 3;
			showMessage("이동 속도 증가!");
			t = 1;
		}
		else {
			showMessage("더 살 수 없습니다.");
		}
	}
	else if (object == attackspeed) {
		if (t == 0) {
			attack_time = attack_time * 0.8;
			showMessage("공격 속도 증가!");
			t = 1;
		}
		else {
			showMessage("더 살 수 없습니다.");
		}
	}
	else if (object == hp) {
		if (t == 0) {
			m_att -= 10;
			showMessage("체력 증가!");
			t = 1;
		}
		else {
			showMessage("더 살 수 없습니다.");
		}
	}
	else if (object == critical) {
		if (t == 0) {
			cri += 10;
			showMessage("치명타 확률 증가!");
			t = 1;
		}
		else {
			showMessage("더 살 수 없습니다.");
		}
	}
	else if (object == move) {
		if (t == 0) {
			skill1 = true;
			hideObject(move);
			showMessage("순간이동 스킬 획득!");
			t = 1;
		}
		else {
			showMessage("더 살 수 없습니다.");
		}
	}
	else if (object == stun) {
		if (t == 0) {
			skill2 = true;
			hideObject(stun);
			showMessage("물풍선 스킬 획득!");
			t = 1;
		}
		else {
			showMessage("더 살 수 없습니다.");
		}
	}
	else if (object == shield) {
		if (t == 0) {
			skill3 = true;
			hideObject(shield);
			showMessage("방어막 스킬 획득!");
			t = 1;
		}
		else {
			showMessage("더 살 수 없습니다.");
		}
	}
	else if (object == gamestartbutton) {
		skills();
		startgamescene();
	}
}


// 순간이동 스킬
void teleport(KeyCode code)
{
	if (code == KeyCode::KEY_RIGHT_ARROW) {
		playerX += SKILL1_STEP;
		locateObject(player, gamescene, playerX, playerY);
		setTimer(skill1_timer, SKILL1_TIME);
		startTimer(skill1_timer);
		showObject(cooldown1);
	}
	else if (code == KeyCode::KEY_LEFT_ARROW) {
		playerX -= SKILL1_STEP;
		locateObject(player, gamescene, playerX, playerY);
		setTimer(skill1_timer, SKILL1_TIME);
		startTimer(skill1_timer);
		showObject(cooldown1);
	}
}

// 물풍선 스킬
void waterballon()
{
	damaged = true;
	hideObject(m);
	showObject(m_bubble);

	hideObject(beam);
	hideObject(warning);
	hideObject(half_beam);

	stopTimer(m_timer1);
	stopTimer(m_timer2);
	stopTimer(m_timer3);
	stopTimer(m_timer4);

	showObject(cooldown2);

	setTimer(skill2_timer, SKILL2_TIME);
	startTimer(skill2_timer);
}

// 방어막 스킬
void barrier()
{
	setTimer(skill3_timer1, SKILL3_TIME);
	startTimer(skill3_timer1);
	startTimer(skill3_timer2);
	showObject(cooldown3);
	showObject(p_shield);

	setTimer(barrier_timer, BARRIER_TIME);
	startTimer(barrier_timer);
	damaged = true;
}

void player_mouseCallback(ObjectID object, int x, int y, MouseAction action)
{
	if (object == skillButton1) {
		teleport(c);
	}
	else if (object == skillButton2) {
		waterballon();
	}
	else if (object == skillButton3) {
		barrier();
	}
}

void player_keyboardCallback(KeyCode code, KeyState state)
{
	if (code == KeyCode::KEY_RIGHT_ARROW) {
		dx += (state == KeyState::KEY_PRESSED ? spe : -spe);
		c = code;
	}
	else if (code == KeyCode::KEY_LEFT_ARROW) {
		dx -= (state == KeyState::KEY_PRESSED ? spe : -spe);
		c = code;
	}

}

void player_timerCallback(TimerID timer)
{
	if (timer == barrier_timer) {
		damaged = false;
	}

	if (timer == player_animation) {						//플레이어 움직임
		playerX += dx;
		if (playerX < 0) playerX = 0;
		if (playerX > 1120) playerX = 1120;

		locateObject(player, gamescene, playerX, playerY);
		locateObject(p_shield, gamescene, playerX, playerY);

		setTimer(timer, ANIMATION_TIME);
		startTimer(timer);
	}

	if (timer == attack_animation0) {
		setTimer(attack_animation1, ATTACK_TIME);
		startTimer(attack_animation1);
	}
	
	if (timer == attack_animation1) {		//공격 애니메이션

		critic();
		doll_p_check = false;
		attackX = playerX + 10;

		doll_check();

		if (doll_p_check == true) {
			locateObject(p_half_attack, gamescene, attackX, attackY);
			showObject(p_half_attack);
			doll_p_check = false;
		}
		else {
			locateObject(p_attack, gamescene, attackX, attackY);
			showObject(p_attack);
			attackshown = true;
			damage_check();
		}

		setTimer(timer, ATTACK_TIME);
		startTimer(timer);

		startTimer(attack_animation2);
	}
	if (timer == attack_animation2) {
		hideObject(p_attack);
		if (cr == true) {
			att = att / 2;
			cr = false;
		}
		attackshown = false;
		hideObject(p_half_attack);
		setTimer(timer, ATTACK_SHOWN_TIME);
	}

	//스킬 쿨타임
	if (timer == skill1_timer) {							//순간이동
		
		hideObject(cooldown1);
	}
	if (timer == skill2_timer) {                            //물풍선
		hideObject(m_bubble);
		showObject(m);
		damaged = false;
		
		//물풍선에서 몬스터가 나오고난후 스킬 쿨타임 
		setTimer(after_timer, 10.f);
		startTimer(after_timer);

		setTimer(m_timer1, 2.f);
		startTimer(m_timer1);
	}
	if (timer == after_timer)
		hideObject(cooldown2);

	if (timer == skill3_timer1) {                            //방어막
		hideObject(cooldown3);
	}
	if (timer == skill3_timer2) {
		hideObject(p_shield);
		damaged = false;
		setTimer(timer, BARRIER_TIME);
	}
	if (timer == skill4_timer) {							//치명타

		hideObject(cooldown1);
	}
}

void mouseCallback(ObjectID object, int x, int y, MouseAction action)
{
	player_mouseCallback(object, x, y, action);
	lobby_mouseCallback(object, x, y, action);
}

void timerCallback(TimerID timer)
{
	lobby_timerCallback(timer);
	player_timerCallback(timer);
	doll_timerCallback(timer);
	monster_timerCallback(timer);
}

ObjectID createObject(const char* image, SceneID scene, int x, int y, bool shown = true)
{
	ObjectID object = createObject(image);

	locateObject(object, scene, x, y);

	if (shown) {
		showObject(object);
	}

	return object;
}

int main()
{
	setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);
	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);

	srand(time(0));

	setMouseCallback(mouseCallback);
	setKeyboardCallback(player_keyboardCallback);
	setTimerCallback(timerCallback);

	start = createScene("start", "images/start.png");
	store = createScene("store", "images/store.png");
	gamescene = createScene("gamescene", "images/game.png");
	m = createObject("images/monster.png", gamescene, mx, 330, true);
	hill = createObject("images/hill.png", gamescene, 0, 0, true);
	m_bubble = createObject("Images/m_bubble.png", gamescene, mx, 370, false);

	m_HP_bar_back = createObject("barback.png", gamescene, 910, 650);
	m_HP_bar = createObject("monsterHP.png", gamescene, m_HP_barX, 650);

	p_HP_bar_back = createObject("barback.png", gamescene, 910, 0);
	p_HP_bar = createObject("playerHP.png", gamescene, m_HP_barX, 0);

	startbutton = createObject("images/startbutton.png", start, 0, 0, true);
	attack = createObject("images/attack.png", store, 600, 400, true);
	speed = createObject("images/speed.png", store, 750, 400, true);
	attackspeed = createObject("images/attackspeed.png", store, 900, 400, true);
	hp = createObject("images/hp.png", store, 1050, 400, true);
	critical = createObject("images/critical.png", store, 600, 200, true);
	move = createObject("images/move.png", store, 750, 200, true);
	stun = createObject("images/stun.png", store, 900, 200, true);
	shield = createObject("images/shield.png", store, 1050, 200, true);
	stp1 = createObject("images/stp1.png", store, 50, 0, true);
	stp2 = createObject("images/stp2.png", store, 50, 0, false);
	gamestartbutton = createObject("images/startbutton.png", store, 900, 50, true);

	doll = createObject("images/doll.png", gamescene, 0, 255, true);

	player = createObject("Images/player.png", gamescene, playerX, playerY, true);
	p_attack = createObject("Images/p_attack.png", gamescene, attackX, attackY, false);
	p_half_attack = createObject("Images/p_half_attack.png", gamescene, attackX, attackY, false);
	skillButton1 = createObject("Images/skill1.jpg", gamescene, 800, 0, false);
	skillButton2 = createObject("Images/skill2.jpg", gamescene, 860, 0, false);
	skillButton3 = createObject("Images/skill3.jpg", gamescene, 920, 0, false);
	cooldown1 = createObject("Images/cool_down.png", gamescene, 800, 0, false);
	cooldown2 = createObject("Images/cool_down.png", gamescene, 860, 0, false);
	cooldown3 = createObject("Images/cool_down.png", gamescene, 920, 0, false);
	p_shield = createObject("Images/shiel.png", gamescene, 0, 0, false);


	mx = 500;
	beam = createObject("images/beam.png", gamescene, mx + 10, 0, false);
	warning = createObject("images/warning.png", gamescene, mx, 0, false);
	half_beam = createObject("images/halfbeam.png", gamescene, mx, 335, false);

	m_HP_bar = createObject("images/m_hp.png", gamescene, m_HP_barX, m_HP_barY);
	p_HP_bar = createObject("images/m_hp.png", gamescene, p_HP_barX, p_HP_barY);




	//로비 타이머
	lobby_timer1 = createTimer(0.2f);
	lobby_timer2 = createTimer(0.2f);
	startTimer(lobby_timer1);

	//몬스터 관련 타이머
	m_timer1 = createTimer(2.f);
	m_timer2 = createTimer(0.5f);
	m_timer3 = createTimer(0.5f);
	m_timer4 = createTimer(0.1f);

	//플레이어 관련 타이머
	player_animation = createTimer(ANIMATION_TIME);
	attack_animation0 = createTimer(2.f);
	attack_animation1 = createTimer(ATTACK_TIME);
	attack_animation2 = createTimer(ATTACK_SHOWN_TIME);
	skill1_timer = createTimer(SKILL1_TIME);
	skill2_timer = createTimer(SKILL2_TIME);
	skill2_timer2 = createTimer(0.5f);
	skill3_timer1 = createTimer(SKILL3_TIME);
	skill3_timer2 = createTimer(BARRIER_TIME);
	skill4_timer = createTimer(CRITICAL_TIME);
	check_timer = createTimer(CHECK_TIME);


	//돌거북 관련 타이머
	d_timer1 = createTimer(0.01f);
	d_timer2 = createTimer(0.01f);

	after_timer = createTimer(10.0f);

	startGame(start);
}