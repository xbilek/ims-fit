#include "simlib.h"
#include <ctime>

const int MINUTA = 60;
const int HODINA = 60*MINUTA;
const int DEN = 24*HODINA;
const int MESIC = 30*DEN;
const int ROK = 12*MESIC;
const int DOBA_OTEVRENI = 2*HODINA;
const int INTERVAL_PRERUSENI = 2 * DOBA_OTEVRENI + 4*HODINA;

const double DOBA_SIMULACE = 1*ROK;

bool dobre_pocasi = true;
bool muze_tanker = true;


Stat odbaveni_lode;

int pocet_priplouvajicich_cargo = 0;
int pocet_priplouvajicich_tankeru = 0;
int pocet_odplouvajicich_cargo = 0;
int pocet_odplouvajicich_tankeru = 0;
int pocet_vypadku_pocasi = 0;
int pocet_priplouvajicich_tankeru_velkych = 0;
int pocet_odplouvajicich_tankeru_velkych = 0;

Facility hlavni_linka("hlavni linka");
Facility tanker_linka("tanker linka");
Facility velky_tanker_linka("velky tanker linka");

class Cargo : public Process {
    void Behavior(){
        double pripluti = Time;
        Seize(hlavni_linka);
        Wait(10 * MINUTA);
        //Print("Skoncil wait 10 minut v procesu cargo\n");
        pocet_odplouvajicich_cargo++;
        Release(hlavni_linka);
        odbaveni_lode(Time - pripluti);
    }
};

class Tanker : public Process {
    void Behavior(){
        Priority = 99;
        Seize(tanker_linka);
        Seize(hlavni_linka); 
        Wait(10 * MINUTA);
        pocet_odplouvajicich_tankeru++;
        Release(hlavni_linka);
        Wait(5* MINUTA);
        Release(tanker_linka);
        //Print("Skoncil wait 10 minut v procesu cargo\n");
        
    }
};

class Tanker_velky : public Process {
    void Behavior(){
        Priority = 99;
        Seize(velky_tanker_linka);
        Seize(hlavni_linka); 
        Wait(10 * MINUTA);
        pocet_odplouvajicich_tankeru_velkych++;
        Release(hlavni_linka);
        Wait(80 * MINUTA);
        Release(velky_tanker_linka);
    }
};

class Preruseni : public Process {
    void Behavior(){
        Priority = 150;
        Seize(hlavni_linka);
        Wait(DOBA_OTEVRENI + 4 * HODINA);
        Release(hlavni_linka);

    }
};


class Pocasi : public Process {
    void Behavior(){
        Priority = 200;
        Seize(hlavni_linka);
        dobre_pocasi = false;
        Wait(Normal(105*HODINA, 10 * HODINA));
        dobre_pocasi = true;
        Release(hlavni_linka);
       
    }
};

class gen_preruseni : public Event{
    void Behavior(){
        (new Preruseni)->Activate();
        Activate(Time + INTERVAL_PRERUSENI);
    }

};


class gen_cargo : public Event{
    void Behavior(){
        pocet_priplouvajicich_cargo++;
        (new Cargo)->Activate();
         if(dobre_pocasi)
           // Activate(Time+Exponential(1*HODINA));
           Activate(Time+Exponential(0.5*HODINA));
        else
            Activate(Time+Exponential(7*DEN));;
    }

};

class gen_tanker : public Event{
    void Behavior(){
        pocet_priplouvajicich_tankeru++;
        (new Tanker)->Activate();
        if(dobre_pocasi)
            Activate(Time+Exponential(2*HODINA));
        else
            Activate(Time+Exponential(7*DEN));
    }

};

class gen_tanker_velky : public Event{
    void Behavior(){
        pocet_priplouvajicich_tankeru_velkych++;
        (new Tanker_velky)->Activate();
        if(dobre_pocasi)
            Activate(Time+Exponential(12*HODINA));
        else
            Activate(Time+Exponential(7*DEN));
    }

};


class gen_spatne_pocasi : public Event{
    void Behavior(){
        pocet_vypadku_pocasi++;
        (new Pocasi)->Activate();
        Activate(Time+100 * DEN);
    }
};

int main(){
    RandomSeed(time(NULL));
    Init(0,DOBA_SIMULACE);
    (new gen_cargo)->Activate(Time+Exponential(20*MINUTA));
    (new gen_tanker)->Activate(Time+Exponential(40*MINUTA));
    (new gen_preruseni)->Activate(Time + 2*HODINA);
    (new gen_tanker_velky)->Activate(Time + 6*HODINA);

    Run();

    odbaveni_lode.Output();

    Print("Pocet priplouvajicich cargo: %d\n", pocet_priplouvajicich_cargo);
    Print("Pocet odplouvajicich cargo: %d\n", pocet_odplouvajicich_cargo);
    Print("Pocet priplouvajicich tankeru: %d\n", pocet_priplouvajicich_tankeru);
    Print("Pocet odplouvajicich tankeru: %d\n", pocet_odplouvajicich_tankeru);
    Print("Pocet priplouvajicich tankeru velkych: %d\n", pocet_priplouvajicich_tankeru_velkych);
    Print("Pocet odplouvajicich tankeru velkych: %d\n", pocet_odplouvajicich_tankeru_velkych);


}