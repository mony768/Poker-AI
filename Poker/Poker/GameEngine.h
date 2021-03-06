#pragma once

#include "Ref.h"

//vygenerovani botu s danou toppologii
	//prida jim zakladni vklad ke stollu a to je 1000
	//na treninku maji limit 1000, abz boti nehazeli neuveritelne satzky, ale aby se furt trenovali, tak kdyz prohrajou tak se jim prictou dalsi penize ale furt budou v minusu
/*void vytvorenibota(vector<NN>& boti, vector<unsigned> topology, int pocetbotu) {
	for (unsigned i = 0; i < pocetbotu; i++)
	{
		clock_t cas;
		cas = clock();
		boti.push_back(NN(topology,i));
		boti[i].mena = 10000;
		boti[i].vsazeno = 0;
		boti[i].allin = false;
		boti[i].risnultohlekolo = false;
		boti[i].slozil = false;
		cout << "Bot byl uspesne vygenerovan!!" << endl << "Jeho id: " << i << endl;
		double duration = (clock() - cas) / (double)CLOCKS_PER_SEC;
		cout << "Cas deklarace bota: " << duration << endl;
	}
}
*/


void stats_reset(vector<NN>& boti) {
	for (int i = 0; i < boti.size(); i++)
	{
		boti[i].profit = 0;
		boti[i].mena = 10000;
		boti[i].risnultohlekolo = false;
	}
}

//pripravi input pro jednotliveho hrace ktery je na tahu
void Game(vector<double>& input,NN& bot, Ref& mozek, int tah, int cast_hry) {

	input[0] = mozek.hraci[1].prvnikarta;
	input[1] = mozek.hraci[1].druhakarta;
	switch (cast_hry)
	{
	case 0:
		for (int i = 0; i < 5; i++)
		{
			input[i + 2] = -1;
		}
		break;
	case 1:
		for (int i = 0; i < 3; i++)
		{
			input[i + 2] = mozek.stul[i];
		}
		input[5] = -1;
		input[6] = -1;
		break;
	case 2:
		for (int i = 0; i < 4; i++)
		{
			input[i + 2] = mozek.stul[i];
		}
		input[6] = -1;
		break;
	default:
		for (int i = 0; i < 5; i++)
		{
			input[i + 2] = mozek.stul[i];
		}
		break;
	}

	input[7] = bot.mena;
	input[8] = mozek.kombinace(cast_hry, tah % 2);
}

void reset_raise(NN& bot, Player& player) {
	bot.risnultohlekolo = false;
	player.risnultohlekolo = false;
}

//vrati akci kterou bot chce zahrat
int akce(vector<double>& vysledky) {
	double vysledek = 0.0;
	int vysledek_id = 0;
	for (unsigned i = 0; i < vysledky.size(); i++)
	{
		if (vysledky[i] >= vysledek)
		{
			vysledek = vysledky[i];
			vysledek_id = i;
		}
	}
	return vysledek_id;
}


void akce_bota(vector<double>& input, vector<double>& results, Ref& mozek, Player& player,NN& bot, int cast_hry, int& i) {
		if (bot.slozil == true || player.slozil == true)
		{
			i = 3;
		}
		else
		{
			//cout << "tah hrace c: " << abs(tah - 1) << endl;
			Game(input, bot, mozek, 1, cast_hry);

			bot.input(input);
			bot.vysledek(results);
			int ak = akce(results);
			if (player.allin == false)
			{
				//nemuze prihodt viskrat behem ednoho tahuy, tak automaticky kdyz bude chtit pridhodit tak jenom dohodi danou castku
				if (bot.risnultohlekolo == true && (ak == 3 || ak == 1))
				{
					ak = 0;
				}
				if (mozek.pot - 2 * bot.vsazeno > bot.mena && (ak == 3 || ak == 1))
				{
					ak = 0;
				}
				//da all in
				if (ak == 3)
				{
					mozek.sazka(bot, 0);
					cout << "allin" << endl;
					//muyysime odebrat hodnotu i, aby mohl zareagovat na danou situaci i druhy hrac
					i--;
				}
				//zahodi karty
				else if (ak == 2) {
					/*mozek.vyhra(boti[tah], bot, 1);*/
					i = 2;
					mozek.tah = "end";
					bot.slozil = true;
					cout << "zahodil" << endl;
				}
				//risne castku
				else if (ak == 1)
				{
					//prihodi castku ktera je random + castka kterou musim minimalne dorovnat
					int rise = (rand() % bot.mena / 2 + 10) + mozek.pot - 2 * bot.vsazeno;
					if (rise > player.mena)
					{
						rise = player.mena;
					}
					mozek.sazka(bot, rise);
					bot.risnultohlekolo = true;
					cout << "rise s castkou: " << rise << endl;
					//muyysime odebrat hodnotu i, aby mohl zareagovat na danou situaci i druhy hrac
					i--;
				}
				//checkne/callne
				else
				{
					if (mozek.pot / 2 > bot.vsazeno&& bot.allin == false)
					{
						int sazka = mozek.pot - 2 * bot.vsazeno;
						if (sazka >= bot.mena)
						{
							mozek.sazka(bot, 0);
							cout << "dorovnal all inem" << endl;
							i = 2;
						}
						else
						{
							mozek.sazka(bot, sazka);
							cout << "dorovnal" << endl;
						}
					}
					else
					{
						cout << "chacknul" << endl;
					}
				}
			}
			//pokud druhej dal all in
			else
			{
				//zahodi karty
				if (ak == 2) {
					/*mozek.vyhra(boti[tah], bot, 1);*/
					i = 2;
					mozek.tah = "end";
					bot.slozil = true;
					cout << "zahodil" << endl;
				}
				//dorovna
				else
				{

					int sazka = mozek.pot - 2 * bot.vsazeno;
					if (sazka >= bot.mena)
					{
						mozek.sazka(bot, 0);
						cout << "dorovnal all inem" << endl;
					}
					else
					{
						mozek.sazka(bot, sazka);
						cout << "dorovnal" << endl;
					}

				}
			}
		
		}
}

//urci kdo zacina hru
int KdoJeBig(bool big) {
	if (big == true)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}



void vypis(string text, vector <double>& hodnoty) {
	cout << text << endl;
	for (unsigned i = 0; i < hodnoty.size(); ++i)
	{
		cout << hodnoty[i] << " ";
	}

	cout << endl;
}
