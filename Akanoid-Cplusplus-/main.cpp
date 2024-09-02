/* LIBRERIAS */
#include <allegro.h>
#include <string>
#include <conio.h>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <winalleg.h>
#include <fstream>

/* DEFINICION DE VENTANA*/
#define ancho 1024
#define alto 720

/* DEFINICION DE VARIABLES GLOBALES*/
#define velocidadInicial 3

/* OBJETOS */
MIDI    *MusicaInicio;
MIDI    *MusicaJuego;
SAMPLE  *SonidoInicioJuegoSelect;
SAMPLE  *SonidoInicioNivel;
SAMPLE  *SonidoLadrilloRoto;
SAMPLE  *SonidoReboteBola;
SAMPLE  *SonidoRevivir;
SAMPLE  *SonidoVidaExtra;
SAMPLE  *SonidoReboteBolaPared;
SAMPLE  *SonidoVidaPerdida;
SAMPLE  *SonidoBoteBolaBase;
SAMPLE  *SonidoGameOver;


/* FUENTES */
DATAFILE *datafile;
FONT     *Arkarnoid;
FONT     *NTRS;

/* IMAGENES */
BITMAP  *buffer;
BITMAP  *logo;
BITMAP  *panel;
BITMAP  *recuadro;
BITMAP  *fondo1;
BITMAP  *fondo2;
BITMAP  *fondo3;
BITMAP  *fondo4;
BITMAP  *fondo5;
BITMAP  *gameover;
BITMAP  *pelota;
BITMAP  *lad1;
BITMAP  *lad2;
BITMAP  *lad3;
BITMAP  *lad4;
BITMAP  *lad5;
BITMAP  *lad6;
BITMAP  *lad7;
BITMAP  *ladd;
BITMAP  *base;
BITMAP  *base2;
BITMAP  *base3;
BITMAP  *base4;

/* BLOQUES */
using namespace std;

/* VARIABLES */
int     retardo = 100;
int     vidas =3;
int     level =1;
int     score=0;
int     dirY=-1;
int     dirX=1;
int     velocidad=3;
int     fondo =1;
bool    juegoiniciado=false;
bool    finJuego=false;
bool    fin =false;
bool    nuevonivel=false;
bool    enjuego=false;
bool    muerte=false;
int     secuenciamuerte=1;
bool    musica=true;
bool    efectos=true;
bool    existearchivo=false;
int     higscore=0;
int     baseX=255;
int     bolaX=295;
int     bolaY=650;
int     bordeizquiera;
int     bordederecha;
int     colBola;
int     filaBola;
int     elemento;
int     fila[]={20,50,80,110,140,170,200};

/* PANTALLAS */
int     pant1[63]={1,1,1,1,1,1,1,1,1,
                   2,2,2,2,2,2,2,2,2,
                   3,3,3,3,3,3,3,3,3,
                   4,4,4,4,4,4,4,4,4,
                   5,5,5,5,5,5,5,5,5,
                   6,6,6,6,6,6,6,6,6,
                   7,7,7,7,7,7,7,7,7};

int     pant2[63]={1,2,3,4,0,0,5,6,7,
                   5,6,7,0,0,1,2,3,4,
                   1,3,4,0,0,3,3,3,3,
                   4,4,4,4,5,5,5,5,5,
                   2,0,0,1,1,0,0,7,7,
                   3,4,5,6,7,1,2,0,0,
                   0,0,0,7,7,2,1,0,0};

/* *** */
int     mapa[63]; /*    PANTALLA AUXILIAR   */


/* DECLARACIONES*/
int     inicializar ();
void    inicilizarSonidos();
void    inicilizarPantalla();
void    armadoPantalla ();
void    inicializarJuegoData();
void    inicializarNivel();
void    validacionBase();
void    muestraLadrillo();
void    nuevaBola();
void    teclasSonido();
void    jugar();
void    ConfiguraNivel();
void    RetomarJuego();
int     cuentoLadrillo ();
void    dibujaMuerte();
void    destruyo_componentes();
void    cargo_archivo();
void    grabo_archivo();


/*  INICIO */
int main (){
    try{

        if(inicializar() == 1) return 1;//VERIFICAR EL SISTEMA DE SONIDO , EN CASO DE ERROR CIERRA TODO

        while(!fin){//CICLO CONTINUO MIENTRAS NO TERMINE EL JUEGO

                armadoPantalla();//CARGAR LOS DATOS A PANTALLA

                if( key[KEY_ESC] )fin =true;//EN CASO DE PRESIONAR ESC , SE CIERRA EL JUEGO
                if( key[KEY_ENTER]&& !juegoiniciado ){//EN CASO DE PRESIONAR ENTER , INICIA EL JUEGO

                   jugar();//LLAMAR A FUNCION PARA INICIAR JUEGO
                midi_pause();
                if(efectos){
                    play_sample(SonidoGameOver,200,150,1000,0);
                }

                if(score>higscore){

                        higscore=score;
                        grabo_archivo();

                }
                while(!key[KEY_ESC] && !key[KEY_ENTER]){

                }

                    vidas=3;
                    level=1;
                    velocidad=velocidadInicial;
                    score=0;

            }
        }

        destruyo_componentes();

    }catch (exception& e){

        cout << e.what() << '\n';

    }

    return 0;

}
END_OF_MAIN();

/* FUNCIONES */
/* DETECTAR TARJETA DE SONIDO, TECLADO E INICIALIZAR ALLEGRO */
int inicializar (){

    allegro_init();//INICIALIZAR ALLEGRO
    install_keyboard();//INICIALIZAR TECLADO

    set_window_title("Arkanoid -By Pykana-");//TITULO DE LA VENTANA DE JUEGO

    if(install_sound(DIGI_AUTODETECT,MIDI_AUTODETECT,NULL)!=0 ){//DETECTAR TARJETA SONIDO

        allegro_message("Error !  Inicializando sistema de sonido \n\n\n" , allegro_error);//MENSAJE EN CASO DE ERROR
        return 1;//RETORNA 1 EN CASO DE ERRORES

    }

    /* **** */
    inicilizarPantalla();//INICIA LA CARGA DE IMAGENES
    inicilizarSonidos();//INICIA LA CARGA DE SONIDOS

    cargo_archivo();

    /* CARGA DE FUENTES */
    datafile=load_datafile("font/Fuentes.dat");//CARGA DE FUENTES DESDE EL ARCHIVO GENERADO POR TTF2PCX Y GRABBER
    Arkarnoid=(FONT*)datafile[0].dat;//CARGA LA FUENTE DE LA POSICION 0 Y LO ASIGNA A ARKANOID
    NTRS=(FONT*)datafile[1].dat;//CARGA LA FUENTE DE LA POSICION 1 Y LO ASIGNA A NTRS

    play_midi(MusicaInicio,0);
    return 0;//RETORNA 0 EN CASO DE NO TENER ERRORES

}

/* CARGAR SONIDOS A MEMORIA */
void inicilizarSonidos(){

    set_volume(230,200);//INICIALIZAR VOLUMEN

    MusicaInicio                =load_midi("bgm/ark.mid");//CARGAR ARCHIVO MIDI DE LA RUTA ESTABLECIDA
    MusicaJuego                 =load_midi("bgm/Arkanoid.mid");//CARGAR ARCHIVO MIDI DE LA RUTA ESTABLECIDA
    SonidoInicioJuegoSelect     =load_wav("bgm/InicioJuego.wav");//CARGAR ARCHIVO WAV DE LA RUTA ESTABLECIDA
    SonidoInicioNivel           =load_wav("bgm/inicioNivel.wav");//CARGAR ARCHIVO WAV DE LA RUTA ESTABLECIDA
    SonidoLadrilloRoto          =load_wav("bgm/ladrilloRoto.wav");//CARGAR ARCHIVO WAV DE LA RUTA ESTABLECIDA
    SonidoReboteBola            =load_wav("bgm/rebotePelota.wav");//CARGAR ARCHIVO WAV DE LA RUTA ESTABLECIDA
    SonidoVidaExtra             =load_wav("bgm/vidaExtra.wav");//CARGAR ARCHIVO WAV DE LA RUTA ESTABLECIDA
    SonidoVidaPerdida           =load_wav("bgm/fallo.wav");//CARGAR ARCHIVO WAV DE LA RUTA ESTABLECIDA
    SonidoGameOver              =load_wav("bgm/SonidoGameOver.wav");//CARGAR ARCHIVO WAV DE LA RUTA ESTABLECIDA
    SonidoRevivir               =load_wav("bgm/revivir.wav");//CARGAR ARCHIVO WAV DE LA RUTA ESTABLECIDA
    SonidoReboteBolaPared       =load_wav("bgm/rebotaParedes.wav");//CARGAR ARCHIVO WAV DE LA RUTA ESTABLECIDA
    SonidoBoteBolaBase          =load_wav("bgm/reboteBase.wav");//CARGAR ARCHIVO WAV DE LA RUTA ESTABLECIDA

}

void destruyo_componentes(){

    destroy_midi(MusicaJuego);
    destroy_sample(SonidoInicioJuegoSelect);
    destroy_sample(SonidoInicioNivel);
    destroy_sample(SonidoLadrilloRoto);
    destroy_sample(SonidoReboteBola);
    destroy_sample(SonidoBoteBolaBase);
    destroy_sample(SonidoReboteBolaPared);
    destroy_sample(SonidoVidaExtra);
    destroy_sample(SonidoVidaPerdida);
    destroy_sample(SonidoGameOver);
	destroy_bitmap(buffer);

}

/* CARGA ELEMENTOS DE MEMORIA */
void inicilizarPantalla(){

    set_color_depth(32);//PROFUNDIDAD DE COLOR DE 32 BITS

    set_gfx_mode(GFX_AUTODETECT,ancho,alto,0,0);//ALLEGRO - AUTODETECTAR EL CONTROLADOR DE GRAFICOS Y DEFINIR LAS DIMENSIONES DE LA PANTALLA  (X,Y,0,0)  LOS OTROS DOS 0 SE USAN PARA SCROLLING , EN CASO DE QUERER AÑADIR PIXELES OCULTOS

    buffer=create_bitmap(ancho,alto);//CREACION DE BUFFER CON LAS DIMENSIONES ESTABLECIDAS

    logo=load_bitmap("img/logo.bmp", NULL);//CARGAR BITMAP DESDE LA RUTA ESTABLECIDA
    panel=load_bitmap("img/panel.bmp", NULL);//CARGAR BITMAP DESDE LA RUTA ESTABLECIDA
    recuadro=load_bitmap("img/recuadro.bmp", NULL);//CARGAR BITMAP DESDE LA RUTA ESTABLECIDA
    gameover=load_bitmap("img/gameover.bmp", NULL);//CARGAR BITMAP DESDE LA RUTA ESTABLECIDA
    pelota=load_bitmap("img/pelota.bmp",NULL);

    /*FONDOS*/
    fondo1=load_bitmap("img/fondo1.bmp", NULL);//CARGAR BITMAP DESDE LA RUTA ESTABLECIDA
      fondo2=load_bitmap("img/fondo2.bmp", NULL);//CARGAR BITMAP DESDE LA RUTA ESTABLECIDA
        fondo3=load_bitmap("img/fondo3.bmp", NULL);//CARGAR BITMAP DESDE LA RUTA ESTABLECIDA
          fondo4=load_bitmap("img/fondo4.bmp", NULL);//CARGAR BITMAP DESDE LA RUTA ESTABLECIDA
            fondo5=load_bitmap("img/fondo5.bmp", NULL);//CARGAR BITMAP DESDE LA RUTA ESTABLECIDA

    /*LADRILLOS*/
    lad1=load_bitmap("img/ladrillo1.bmp", NULL);//CARGAR BITMAP DESDE LA RUTA ESTABLECIDA
     lad2=load_bitmap("img/ladrillo2.bmp", NULL);//CARGAR BITMAP DESDE LA RUTA ESTABLECIDA
      lad3=load_bitmap("img/ladrillo3.bmp", NULL);//CARGAR BITMAP DESDE LA RUTA ESTABLECIDA
       lad4=load_bitmap("img/ladrillo4.bmp", NULL);//CARGAR BITMAP DESDE LA RUTA ESTABLECIDA
        lad5=load_bitmap("img/ladrillo5.bmp", NULL);//CARGAR BITMAP DESDE LA RUTA ESTABLECIDA
         lad6=load_bitmap("img/ladrillo6.bmp", NULL);//CARGAR BITMAP DESDE LA RUTA ESTABLECIDA
          lad7=load_bitmap("img/ladrillo7.bmp", NULL);//CARGAR BITMAP DESDE LA RUTA ESTABLECIDA
           ladd=load_bitmap("img/ladrilloduro.bmp", NULL);//CARGAR BITMAP DESDE LA RUTA ESTABLECIDA

    /*RECTANGULO BASE - JUGADOR*/
    base=load_bitmap("img/base.bmp", NULL);//CARGAR BITMAP DESDE LA RUTA ESTABLECIDA
     base2=load_bitmap("img/base2.bmp", NULL);//CARGAR BITMAP DESDE LA RUTA ESTABLECIDA
      base3=load_bitmap("img/base3.bmp", NULL);//CARGAR BITMAP DESDE LA RUTA ESTABLECIDA
       base4=load_bitmap("img/base4.bmp", NULL);//CARGAR BITMAP DESDE LA RUTA ESTABLECIDA

}

/* MOSTRADO DE ELEMENTOS DE PANTALLA */
void armadoPantalla (){

     try
        {

    //clear_to_color(buffer,makecol(0,0,0));//CARGAR COLOR DE BUFFER PERO USANDO LA FUNCIONA MAKECOL PARA LOS COLORES EN LUGAR HEX
    clear_to_color(buffer,0x000000);//CARGAR COLOR DE BUFFER CON EL COLOR EN DATOS HEXADECIMAL
    draw_sprite(buffer,logo,610,5);//CARGAR SPRITE EN LA POSICION ESTABLECIDA       (X),  (Y)
    draw_sprite(buffer,panel,620,140);//CARGAR SPRITE EN LA POSICION ESTABLECIDA    (X),  (Y)

    /* TEXTO EN PANTALLA */
    textprintf_ex(panel,NTRS,170,23,makecol(0,0,0),makecol(0,0,0),"              ");//DIBUJAR EN PANEL , USAR LETRA NTRS, POSICION 130 (X), POSICION 3(Y), COLOR DE LETRA, COLOR DE FONDO, OBJETO A DIBUJAR
    textprintf_ex(panel,NTRS,170,23,makecol(255,0,0),makecol(0,0,0),"%d",level);//DIBUJAR EN PANEL , USAR LETRA NTRS, POSICION 130 (X), POSICION 3(Y), COLOR DE LETRA, COLOR DE FONDO, OBJETO A DIBUJAR (LEVE)
    textprintf_ex(panel,NTRS,200,85,makecol(0,0,0),makecol(0,0,0),"              ");//DIBUJAR EN PANEL , USAR LETRA NTRS, POSICION 130 (X), POSICION 3(Y), COLOR DE LETRA, COLOR DE FONDO, OBJETO A DIBUJAR
    textprintf_ex(panel,NTRS,200,85,makecol(255,0,0),makecol(0,0,0),"%d",score);//DIBUJAR EN PANEL , USAR LETRA NTRS, POSICION 130 (X), POSICION 3(Y), COLOR DE LETRA, COLOR DE FONDO, OBJETO A DIBUJAR (LEVE)
    textprintf_ex(panel,NTRS,170,150,makecol(0,0,0),makecol(0,0,0),"              ");//DIBUJAR EN PANEL , USAR LETRA NTRS, POSICION 130 (X), POSICION 3(Y), COLOR DE LETRA, COLOR DE FONDO, OBJETO A DIBUJAR
    textprintf_ex(panel,NTRS,170,150,makecol(255,0,0),makecol(0,0,0),"%d",vidas);//DIBUJAR EN PANEL , USAR LETRA NTRS, POSICION 130 (X), POSICION 3(Y), COLOR DE LETRA, COLOR DE FONDO, OBJETO A DIBUJAR (LEVE)

    textprintf_ex(buffer, NTRS, 700, 100, makecol(255,255,255),makecol(0,0,0), "Highscore : %i", higscore);

    /* *** */
    draw_sprite(buffer,recuadro,5,10);//CARGAR SPRITE EN LA POSICION ESTABLECIDA    (X),  (Y)


    switch(fondo){

        case 1:     draw_sprite(buffer,fondo1,11,16);//CARGAR SPRITE EN LA POSICION ESTABLECIDA     (X),  (Y)
            break;
        case 2:     draw_sprite(buffer,fondo2,11,16);//CARGAR SPRITE EN LA POSICION ESTABLECIDA     (X),  (Y)
            break;
        case 3:     draw_sprite(buffer,fondo3,11,16);//CARGAR SPRITE EN LA POSICION ESTABLECIDA     (X),  (Y)
            break;
        case 4:     draw_sprite(buffer,fondo4,11,16);//CARGAR SPRITE EN LA POSICION ESTABLECIDA     (X),  (Y)
            break;
        case 5:     draw_sprite(buffer,fondo5,11,16);//CARGAR SPRITE EN LA POSICION ESTABLECIDA     (X),  (Y)
            break;

    }
       if (!muerte) {

           draw_sprite(buffer,base,baseX,660);//CARGAR SPRITE EN LA POSICION ESTABLECIDA   (X),  (Y)

       }else{

            switch(secuenciamuerte)
           {
           case 1: draw_sprite(buffer,base2,baseX,655);
                   break;
           case 2: draw_sprite(buffer,base3,baseX,650);
                   break;
           case 3: draw_sprite(buffer,base4,baseX,640);
                   break;
           }

       }

    if(enjuego){

            circlefill(buffer, bolaX,bolaY, 10, makecol(124,250,16));

    } else{
            bolaX =baseX+50;
            circlefill(buffer,bolaX,bolaY,10, makecol(124,250,16) );//CREAR CIRULO CON RELLENO EN BUFFER , CON DIMENSIONES (X), (Y), DIAMETRO, COLOR
    }

        muestraLadrillo();//ARMADO DE LADRILLOS

        if(vidas ==0)    draw_sprite(buffer,gameover,150,300);

        blit(buffer,screen,0,0,0,0, ancho,alto);//MANDAR A PANTALLA LAS COSAS CARGADAS A BUFFER , SCREEN (PUEDE IR OTRO BUFFER), COORDENADAS (0,0,0,0), ANCHO Y ALTO DE VENTANA

          }catch (exception& e){

                cout << e.what() << endl;

            }
        }

/* ESTADO DE NUEVO JUEGO */
void inicializarJuegoData(){

        level=1;//EN JUEGO NUEVO INICIA EN NIVEL 1
        finJuego=false;//RESETEA EL ESTADO FIN DE JUEGO A FALSE PARA NUEVA PARTIDA

}

/* CREACION DE ESCENARIO PARA JUEGO */
void inicializarNivel(){

    ConfiguraNivel();//ENTRA A LA CONFIGURACION DEL NIVEL - PANTALLA POR NIVEL
    RetomarJuego();//ENTRA A RETOMAR JUEGO - CAMBIO PARAMETROS A SU ESTADO BASE

    if(efectos){//VERIFICA EL ESTADO ACTUAL DE EFECTOS

        play_sample(SonidoInicioNivel,200,150,1000,0);//ARCHIVO SAMPLE, VOLUMEN, PAN, FRECUENCIA, LOOP

    }

}

/* CONFIGURACION DEL NIVEL */
void ConfiguraNivel(){

   for (int i=0;i<63;i++){//RECORRIDO DE CADA MAPA DE LADRILLOS
          switch (level) {

            case 1:
                mapa[i] = pant1[i];//LEVEL 1 = CARGA PANTALLA1
                break;
            case 2:
                mapa[i] = pant2[i];//LEVEL 2 = CARGA PANTALLA2
                break;
            default:
                mapa[i] = rand() % 9;//LEVEL 3 EN ADELANTE PONES LADRILLOS ALEATORIOS
                break;
                        }
    }

}

/* CONFIGURACION Y VALIDACION DEL ESTADO DEL JUEGO */
void RetomarJuego(){

    baseX=255;//CENTRA LADRIO BASE - JUGADOR
    bolaX=295;//CENTRA BOLA POSICION X
    bolaY=650;//CENTRA BOLA POSICION Y
    enjuego = false;//VUELVE AL ESTADO FALSE - EN ESPERA DEL JUGADOR
    nuevonivel= false;//DETIENE EL ARMADO
    armadoPantalla();//LLAMAR A ARMAR PANTALLA
    velocidad=3+((int)level/5);

}

/* VALIDACION DE LA BASE DEL JUGADOR */
void validacionBase(){

    if(key[KEY_RIGHT]){

        if(baseX < 476) baseX= baseX+velocidad; //DELIMITAR LIMITE DERECHO

    }
    if(key[KEY_LEFT]){

        if(baseX > 11) baseX= baseX-velocidad; //DELIMITAR LIMITE IZQUIERDO
    }

}

/* CONFIGURACION Y MOSTRADO DE LADRILLOS */
void muestraLadrillo(){

    int x,y,col;
    int ladn=0;
    int lad;
    int fila[7]={20,50,80,110,140,170,200};     //POSICIONES DE LADRILLOS

    for(int i=0;i<63;i++){

        if(mapa[i]>0 ){

            lad =   mapa[i];        //SI EL MAPA ES I ES MAYOR QUE 0, ENTONCES MUESTRAS EL LADRILLO I
            y   =   fila[int (i/9)];
            col =   i-(((int)(i/9))*9)+1;
            x   =   13+((col-1)*65);

            switch(lad){
                case 1: draw_sprite(buffer, lad1,x,y);
                break;
                case 2: draw_sprite(buffer, lad2,x,y);
                break;
                case 3: draw_sprite(buffer, lad3,x,y);
                break;
                case 4: draw_sprite(buffer, lad4,x,y);
                break;
                case 5: draw_sprite(buffer, lad5,x,y);
                break;
                case 6: draw_sprite(buffer, lad6,x,y);
                break;
                case 7: draw_sprite(buffer, lad7,x,y);
                break;
                case 8: draw_sprite(buffer, ladd,x,y);
                break;
            }
        }
    }
}

void  dibujaMuerte(){
    muerte = true;
    for(secuenciamuerte =1;secuenciamuerte<5;secuenciamuerte++){

        armadoPantalla();
        Sleep(200);

    }

    muerte = false;
    armadoPantalla();
}


/* CREACION DE LA BOLA */
void nuevaBola(){

     bordeizquiera= baseX+20;
     bordederecha=baseX+100;

    if(bolaY <255){

        filaBola=((int)(((bolaY-20)/30))+1);
        colBola=((int)(bolaX-13)/64)+1;
        elemento = (((filaBola-1)*9)+colBola)-1;

         if(mapa[elemento] !=0 ){

               if(dirY ==1){

                     dirY=-1;

                }else{

                    dirY=1;

                }
                if(mapa[elemento] !=8 ){

                    if(efectos) play_sample(SonidoLadrilloRoto,200,150,1000,0);
                    mapa[elemento]=0;
                    score=score+10;
                    muestraLadrillo();

            }else{

                if(efectos) play_sample(SonidoReboteBola,200,150,1000,0);

            }
         }
    }else{

    if(bolaY>650 && dirY==1){

            if(bolaX >=baseX && bolaX <=baseX+120){

                if(efectos) play_sample(SonidoBoteBolaBase,200,150,1000,0);
                if(bolaX<=bordeizquiera)dirX=-1;
                if(bolaX>=bordederecha) dirX=1;

                dirY=-1;
                return;

            }else{

            if(efectos) play_sample(SonidoVidaPerdida,200,150,1000,0);
            vidas--;
            dibujaMuerte();
            if(vidas>0)  RetomarJuego();
            return;

            }
          }
    }

    if(bolaX>580) dirX=-1;
    if(bolaX<15) dirX=1;
    if(bolaY<15) dirY=1;
    if(bolaX>580 || bolaX<15 || bolaY<15) {

        if(efectos) play_sample(SonidoReboteBola,200,150,1000,0);

    }

    if(dirX==1)bolaX=bolaX+velocidad;
    if(dirX==-1)bolaX=bolaX-velocidad;

    if(dirY==1)bolaY=bolaY+velocidad;
    if(dirY==-1)bolaY=bolaY-velocidad;

    armadoPantalla();
}

/* VALIDACION DE LA TECLAS DE SONIDO */
void teclasSonido(){

      if(key[KEY_DEL]){//SI PRESIONA DEL

        if(musica){//VERIFICA EL ESTADO DE MUSICA

            musica =false;//VALOR MUSICA A FALSE
            midi_pause();//PAUSA MUSICA

        }else{//SI NO

            midi_resume();//RESUME LA MUSICA
            musica=true;//VALOR MUSICA A TRUE

        }
    }
    if(key[KEY_TAB]){//SI PRESIONA TAB

        if(efectos){//VERIFICA EL ESTADO DE MUSICA

        efectos=false;//VALOR EFECTOS A FALSE

        }else{//SI NO

            efectos=true;//VALOR EFECTOS A TRUE

        }
    }
}

int cuentoLadrillo (){

    for (int i=0;i<63;i++){

        if(mapa[i] !=8 && mapa[i] >0) return 1;

    }

    return 0;

}


/*  INTERACCION CON EL JUEGO  */
void jugar(){

        try{

        inicializarJuegoData();//PONER LOS VALORES DEFAULT

        while(!key[KEY_ESC]&& !finJuego){//CICLO CONTINUO MIENTRAS EL JUGADOR JUEGA

            midi_pause();
            inicializarNivel();//CREAR NIVEL

                while(!nuevonivel && !key[KEY_ESC] && vidas>0){//VERIFICA SI EL USUARIO TERMINO EL NIVEL

                        if(key[KEY_SPACE]&& enjuego==false){//VALIDACION DE TECLAS EN CASO EL JUGADOR ESTE JUGANDO

                                if(efectos) stop_sample(SonidoInicioNivel);
                                if(musica) play_midi(MusicaJuego,1);
                                enjuego=true;//INICIA EL JUEGO

                        }

                    validacionBase();//VERIFICA EL ESTADO ACTUAL DE LA BASE QUE MANEJA DEL JUGADOR - LADRILLO

                    if(enjuego){

                        muestraLadrillo();//VERIFICA EL ESTADO ACTUAL DE LOS BLOQUES CON LOS QUE INTERACTUA EL JGADOR
                        nuevaBola();//CREA UNA BOLA EN LA POSICION INICIAL

                    }

                    if(key[KEY_0]){

                        for(int i=0;i<63;i++){

                            mapa[i]=0;

                        }

                    }

                    if(cuentoLadrillo()==0){

                            level++;
                            nuevonivel =true;
                            fondo++;

                            if(fondo==6){

                              fondo =1;
                            }

                            inicializarNivel();
                    }

                    teclasSonido();//VERIFICA SI EL JUGADOR PRESIONO LA TECLA ESPECIFICA PARA MANEJAR EL SONIDO
                    armadoPantalla();//ARMADO DE PANTALLA EN TIEMPO REAL -- CUANDO EL JUGADOR INTERACTUE CON LOS BLOQUES

                    if(vidas ==0) finJuego = true;

                    }
                }
            }catch (exception& e)
            {

            cout << e.what() << endl;

            }
}

void cargo_archivo(){

   ifstream puntaje;                //TIPO DE OBJETO TIPO ARCHIVO  - IMPUT
   char textoPuntaje[100];          //REGISTRO DE ARCHIVO

   puntaje.open("high.dat");        //NOMBRE DE ARCHIVO A ABRIR

   if(puntaje.fail()) {

      existearchivo=false;          //ARCHIVO NO EXISTE
      return;

   }

   if(!puntaje.eof()) {

        puntaje.getline(textoPuntaje, sizeof(puntaje));     //QUE TEXTO PUNTAJE CAMBIE SU DIMENSION ES PUNTAJE
        string s=string(textoPuntaje);                      //CONVERCION DEL PUNTAJE A STRING
        higscore=atoi(s.c_str());                          //TRANSFORMAR EL STRING A DATO NUMERICO

   }

   puntaje.close();                                         //CERRAR ARCHIVO
}

void grabo_archivo(){

    ofstream puntaje;                                       //OUTPUT -
    puntaje.open("high.dat");                               //ABRE Y BORRA CONTENIDO
    puntaje << higscore << endl;                           //PUNTAJE TOMARA EL VALOR DE HIGHSCORE
    puntaje.close();                                        //CIERRA ARCHIVO
}


