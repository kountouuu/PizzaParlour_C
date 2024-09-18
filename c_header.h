#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

//Declaration of needed values --------------------------------------------------<
int num_available_cooks;
int num_available_ovens;
int num_available_deliverer;
int num_available_cashiers;
int ypallhlos_paketarismatos;
int total_failed_payments;

double service_time;
double total_service_time;
double max_service_time;
double waiting_time;
double total_waiting_time;
double max_waiting_time;
double cold_time;
double total_cold_time;
double max_cold_time;
double Total_pizzas_cost;

unsigned int seed;

//Constants & Bounds -----------------------------------------------------------<

const int N_tel = 3; // # of cashiers 
const int N_cook = 2; // # of cooks 
const int N_oven = 10; // # of ovens
const int N_deliverer = 7; // # of delivery guys 

const int T_order_low = 1; // low bound of minutes per order
const int T_order_high = 5; // high bound of minutes per order 

const int N_order_low = 1; //minimum pizzas per order
const int N_order_high = 5; //maximum pizzas per order

const int T_payment_low = 1; // # mins until order was redeemed successful
const int T_payment_high = 2; // # mins until order was redeemed successful 

const int C_pizza = 10; // euros per pizza 

const int T_prep = 1; // time needed for the preperation of a pizza 
const int T_bake = 10; // time needed for pizzas to bake 
const int T_pack = 2; // time needed for the order to get packed and ready to go

const int T_del_low = 5; // minimum minutes for the pizza to be delivered 
const int T_del_high = 15; //maximum minutes for the pizza to be delivered 

//PThread Mutex's --------------------------------------------------------------<
//initializing the mutex's necessary 

pthread_mutex_t mutex_num_available_cooks = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_num_available_cashiers = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_num_available_deliverer = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_num_available_ovens = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_ypallhlos_paketarismatos = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_lock_output = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_total_cold_time = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_max_cold_time = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_total_waiting_time = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_max_waiting_time = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_total_service_time = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_max_service_time = PTHREAD_MUTEX_INITIALIZER;

//Pthread Conditions -----------------------------------------------------------<
//initializing conditions for threads 

pthread_cond_t cond_num_available_cooks = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_num_available_ovens = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_num_available_deliverer = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_num_available_cashiers = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_ypallhlos_paketarismatos = PTHREAD_COND_INITIALIZER;


//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------






