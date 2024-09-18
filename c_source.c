#include "3170195-3180168-3180012.h"

//global scope 
int num_available_cashiers = N_tel;
int num_available_cooks = N_cook;
int num_available_ovens = N_oven;
int num_available_deliverer = N_deliverer;
int ypallhlos_paketarismatos = 1;
int prepared_time = 0;
int delivered_time = 0;

void *startOrder(void *idOfOrder){
	int *id_order;
	id_order = (int *)idOfOrder;
	unsigned int seedp = seed^(*id_order); //unique seed for each thread
	int rc; //run commands var
    struct timespec order_wait; // when the customer makes the call
	struct timespec order_start; // when the order is given
    struct timespec order_bake; // when the order is inserted in the oven
    struct timespec order_stop; // when the order is out of the oven
    struct timespec order_del; // when the order is delivered to the customer
	
    //get the time the order was given
    clock_gettime(CLOCK_REALTIME, &order_wait);
    
	//locking mutex cashiers
    rc = pthread_mutex_lock(&mutex_num_available_cashiers);
    if(rc != 0){
        printf("Mutex: Error %d\n", rc);
        pthread_exit(&rc);
    }
    //condition for no available cashiers
    while(num_available_cashiers<=0){
        rc = pthread_cond_wait(&cond_num_available_cashiers, &mutex_num_available_cashiers);
        if(rc != 0){
            printf("Mutex: Error %d\n", rc);
            pthread_exit(&rc);
        }
    }
    //cashier found for order 
    --num_available_cashiers;

    

	//waiting for transaction to go through with timeRangeOfPayment
    int timeRangeOfPayment = (rand_r(&seedp) % (T_payment_high - T_payment_low + 1)) + T_payment_low;
    sleep(timeRangeOfPayment);
    ++num_available_cashiers;
    
    //unlocking cashier once payment is through & signaling condition
    pthread_cond_signal(&cond_num_available_cashiers);
	rc = pthread_mutex_unlock(&mutex_num_available_cashiers);
    if(rc != 0){
            printf("Mutex: Error %d\n", rc);
            pthread_exit(&rc);
        }
    //get time when order starts being prepared
    clock_gettime(CLOCK_REALTIME, &order_start);
        
    //locking output
    rc = pthread_mutex_lock(&mutex_lock_output);
    if(rc != 0){
        printf("Mutex: Error %d\n", rc);
        pthread_exit(&rc);
    }
    
    //checks payment 
	int transactionStatus = (rand_r(&seedp) % 101);	//transaction status is an int 1-100
	if (transactionStatus <= 5){	//checks if random transaction status int is lower than 5
		printf("Order with ID : %d has failed !\n",*id_order);
		//update waiting time and add it to the total waiting time
        ++total_failed_payments;
        waiting_time = order_wait.tv_sec - order_start.tv_sec;
        //lock total waiting time 
    	rc = pthread_mutex_lock(&mutex_total_waiting_time);
    	if(rc != 0) {
        	printf("Mutex: Error %d\n", rc);
        	pthread_exit(&rc);
    	}
    	total_waiting_time += waiting_time;
    	//unlocking mutex total waiting time
    	rc = pthread_mutex_unlock(&mutex_total_waiting_time);
    	if(rc != 0){
        	printf("Mutex: Error %d\n", rc);
        	pthread_exit(&rc);
   			}
   		//unlocking lock output
        rc = pthread_mutex_unlock(&mutex_lock_output);
    	if(rc != 0){
        	printf("Mutex: Error %d\n", rc);
        	pthread_exit(&rc);
    	}
    	//thread exits if transaction doesnt go through
		pthread_exit(NULL);
		}
	else {	//transaction was successful
		printf("Order with ID : %d was successfully registered !\n", *id_order);
		}
		
	//unlocking output screen
	rc = pthread_mutex_unlock(&mutex_lock_output);
    if(rc != 0){
        printf("Mutex: Error %d\n", rc);
        pthread_exit(&rc);
    }
    
	
    //job now passes down to the cook , because payment was successful
    rc = pthread_mutex_lock(&mutex_num_available_cooks);
    if(rc != 0){
            printf("Mutex: Error %d\n", rc);
            pthread_exit(&rc);
        }
    //checks if there are available cooks
    while ( num_available_cooks <= 0 ) {
    	rc = pthread_cond_wait(&cond_num_available_cooks, &mutex_num_available_cooks);
    	if(rc != 0){
            printf("Mutex: Error %d\n", rc);
            pthread_exit(&rc);
        }
    }
    //found available cook
    --num_available_cooks;
    
    sleep(T_prep); 	// time for the pizzas to be prepared , default value of 2 
    int numberOfPizzas = ( rand_r(&seedp) % ( N_order_high - N_order_low + 1)) + N_order_low; //random # of pizzas

    Total_pizzas_cost += C_pizza * numberOfPizzas;	//update total income from pizzas
    
	//checks if ovens are available 
    while( num_available_ovens <= numberOfPizzas ) {
		rc = pthread_cond_wait(&cond_num_available_ovens, &mutex_num_available_ovens);
		if(rc != 0){
            printf("Mutex: Error %d\n", rc);
            pthread_exit(&rc);
        }
    }
    //found available ovens , discharging the cook
    ++num_available_cooks;
    pthread_cond_signal(&cond_num_available_cooks);
    rc = pthread_mutex_unlock(&mutex_num_available_cooks);
    if(rc != 0){
        printf("Mutex: Error %d\n", rc);
        pthread_exit(&rc);
    }
    //get the time the order is baked
    clock_gettime(CLOCK_REALTIME, &order_bake);
    
	//locking mutex ovens
	rc = pthread_mutex_lock(&mutex_num_available_ovens);
	if(rc != 0){
            printf("Mutex: Error %d\n", rc);
            pthread_exit(&rc);
        }
	//found available ovens 
    num_available_ovens = num_available_ovens - numberOfPizzas;
    
    sleep(T_bake);		//time for the pizzas to bake
    
    num_available_ovens = num_available_ovens + numberOfPizzas;
    //signaling oven condition and unlocking ovens
    pthread_cond_signal(&cond_num_available_ovens);
    rc = pthread_mutex_unlock(&mutex_num_available_ovens);
    if(rc != 0){
        printf("Mutex: Error %d\n", rc);
        pthread_exit(&rc);
    }
	//locking mutex ypallhlou paketarismatos
    rc = pthread_mutex_lock(&mutex_ypallhlos_paketarismatos);
    if(rc != 0){
            printf("Mutex: Error %d\n", rc);
            pthread_exit(&rc);
        }
    //condition gia unavailable ypallhlo paketarismatos
	while(ypallhlos_paketarismatos != 1) {
		rc = pthread_cond_wait(&cond_ypallhlos_paketarismatos, &mutex_ypallhlos_paketarismatos);
		if(rc != 0){
            printf("Mutex: Error %d\n", rc);
            pthread_exit(&rc);
        }
    }
    --ypallhlos_paketarismatos;
    sleep(T_pack);								//time for delivery to be prepared and ready to go
    
    //get the time the order was prepared
	clock_gettime(CLOCK_REALTIME, &order_stop);
	
	//locking output
    rc = pthread_mutex_lock(&mutex_lock_output);
    if(rc != 0){
        printf("Mutex: Error %d\n", rc);
        pthread_exit(&rc);
    }
    //calculating the time that was needed for the order to be prepared
    prepared_time = order_stop.tv_sec - order_wait.tv_sec;
    printf("Order with ID : %d was prepared in %d minutes .\n", *id_order, prepared_time);
	//unlocking output screen
	rc = pthread_mutex_unlock(&mutex_lock_output);
    if(rc != 0){
        printf("Mutex: Error %d\n", rc);
        pthread_exit(&rc);
    }
    
    //unlocking mutex gia ypallhlo paketarismatos
	++ypallhlos_paketarismatos;
	pthread_cond_signal(&cond_ypallhlos_paketarismatos);
	rc = pthread_mutex_unlock(&mutex_ypallhlos_paketarismatos);
    if(rc != 0){
            printf("Mutex: Error %d\n", rc);
            pthread_exit(&rc);
        }

	//time for the delivery to be delivered 
    rc = pthread_mutex_lock(&mutex_num_available_deliverer);
    if ( rc != 0 ){
    	printf("Mutex: Error %d\n", rc);
    	pthread_exit(&rc);
    }
    //condition for not available deliverymen
    while ( num_available_deliverer <= 0 ) {
    	rc = pthread_cond_wait(&cond_num_available_deliverer , &mutex_num_available_deliverer);
    	if ( rc != 0 ){
    		printf("Mutex: Error %d\n", rc);
    		pthread_exit(&rc);
    	}
    }
    //delivery man available
    --num_available_deliverer;
    //unlocking mutex delivery man
    rc = pthread_mutex_unlock(&mutex_num_available_deliverer);
    if ( rc != 0 ){
    	printf("Mutex: Error %d\n", rc);
    	pthread_exit(&rc);
    }
    //wait for the time needed for the pizza to be delivered
    int T_to_and_from_delivery = (rand_r(&seedp) % ( T_del_high - T_del_low + 1)) + T_del_low ; 
    sleep(T_to_and_from_delivery);

    //get the time the order was delivered
    clock_gettime(CLOCK_REALTIME, &order_del);
    
    //locking output
    rc = pthread_mutex_lock(&mutex_lock_output);
    if(rc != 0){
        printf("Mutex: Error %d\n", rc);
        pthread_exit(&rc);
    }
    //calculates the time that was needed for the order to be delivered to the client
    delivered_time = order_del.tv_sec - order_wait.tv_sec ;
    printf("Order with ID : %d was delivered in %d minutes .\n", *id_order, delivered_time);
    
	//unlocking output screen
	rc = pthread_mutex_unlock(&mutex_lock_output);
    if(rc != 0){
        printf("Mutex: Error %d\n", rc);
        pthread_exit(&rc);
    }
    
    //FOOD WAS DELIVERED! deliveryman is returning
    sleep(T_to_and_from_delivery);
    
    //locking mutex deliveryman
    rc = pthread_mutex_lock(&mutex_num_available_deliverer);
    if ( rc != 0 ){
    	printf("Mutex: Error %d\n", rc);
    	pthread_exit(&rc);
    }
    //updates available deliverymen
    ++num_available_deliverer;
    //signaling wait condition for deliverymen , and unlocking mutex
    pthread_cond_signal(&cond_num_available_deliverer);
    rc = pthread_mutex_unlock(&mutex_num_available_deliverer);
    if ( rc != 0 ){
    	printf("Mutex: Error %d\n", rc);
    	pthread_exit(&rc);
    }
    //update total cold time
    rc = pthread_mutex_lock(&mutex_total_cold_time);
    if(rc != 0) {
        printf("Mutex: Error %d\n", rc);
        pthread_exit(&rc);
    }
    //calculates the cold time for the specific order
    cold_time = order_del.tv_sec - order_stop.tv_sec;
    total_cold_time += cold_time;
    
    //update max cold time
    rc = pthread_mutex_lock(&mutex_max_cold_time);
    if(rc != 0) {
        printf("Mutex: Error %d\n", rc);
        pthread_exit(&rc);
    }
    //updates max cold time value
    if(cold_time>max_cold_time){
        max_cold_time = cold_time;
    }
    //unlocking max cold time mutex
    rc = pthread_mutex_unlock(&mutex_max_cold_time);
    if(rc != 0){
        printf("Mutex: Error %d\n", rc);
        pthread_exit(&rc);
    }
	//unlocking total cold time mutex
    rc = pthread_mutex_unlock(&mutex_total_cold_time);
    if(rc != 0){
        printf("Mutex: Error %d\n", rc);
        pthread_exit(&rc);
    }
    //update total service time
    
    rc = pthread_mutex_lock(&mutex_total_service_time);
    if(rc != 0) {
        printf("Mutex: Error %d\n", rc);
        pthread_exit(&rc);
    }
    //calculates service time for specific order
    service_time = order_del.tv_sec - order_wait.tv_sec;
    total_service_time += service_time;
    
    //update max cold time
    rc = pthread_mutex_lock(&mutex_max_service_time);
    if(rc != 0) {
        printf("Mutex: Error %d\n", rc);
        pthread_exit(&rc);
    }
    //updates max service time value
    if(service_time>max_service_time){
        max_service_time = service_time;
    }
    //unlocking max service time mutex
    rc = pthread_mutex_unlock(&mutex_max_service_time);
    if(rc != 0){
        printf("Mutex: Error %d\n", rc);
        pthread_exit(&rc);
    }
    //unlocking total service time value
    rc = pthread_mutex_unlock(&mutex_total_service_time);
    if(rc != 0){
        printf("Mutex: Error %d\n", rc);
        pthread_exit(&rc);
    }
    
    //lock total waiting time 
    rc = pthread_mutex_lock(&mutex_total_waiting_time);
    if(rc != 0) {
        printf("Mutex: Error %d\n", rc);
        pthread_exit(&rc);
    }
    //calculate total waiting time 
    waiting_time = order_start.tv_sec - order_wait.tv_sec;
    total_waiting_time += waiting_time;
    
    //unlocking mutex total waiting time
    rc = pthread_mutex_unlock(&mutex_total_waiting_time);
    if(rc != 0){
        printf("Mutex: Error %d\n", rc);
        pthread_exit(&rc);
    }
    
    //lock max waiting time mutex
    rc = pthread_mutex_lock(&mutex_max_waiting_time);
    if(rc != 0) {
        printf("Mutex: Error %d\n", rc);
        pthread_exit(&rc);
    }
    //find max waiting time value
    if(waiting_time>max_waiting_time){
        max_waiting_time = waiting_time;
    }
    //unlocking max waiting time mutex
    rc = pthread_mutex_unlock(&mutex_max_waiting_time);
    if(rc != 0){
        printf("Mutex: Error %d\n", rc);
        pthread_exit(&rc);
    }

	pthread_exit(idOfOrder);			//thread is finished ! exiting existing thread 
}


int main(int argc, char *argv[]){
	//initialize variables
	//---------------------------------------------------------------------------------
	unsigned int seed;	//initializes seed value to be taken as a parameter
	int run_commands;	//initializes rc value for error checking 
	total_waiting_time=0;
    waiting_time=0;
    max_waiting_time=0;
    total_service_time=0;
    service_time=0;
    max_service_time=0;
    total_cold_time=0;
    cold_time=0;
    max_cold_time=0;
    total_failed_payments=0;
    Total_pizzas_cost=0;
    
    //error checking and assigning parameters
    //---------------------------------------------------------------------------------
    
	if(argc != 3)	{ 						// takes 3 parameters , file and 2 numbers
		printf("Cannot compile , please type the # of Customers and a random # seed .");
		exit(-1);
	}										 
			//registering parameters
	int N_customers = atoi(argv[1]); 		
	seed = atoi(argv[2]);
					
	if(N_customers < 0 )	{			//error msg for <0 param 
		printf("Negative # of customers , please give a positive #."); 
		exit(-1);
	}	
	//continue to the creation of threads and asigning order id for each one									
	//---------------------------------------------------------------------------------
	printf("Welcome to our pizza joint !\n\n");

	int oid[N_customers];					//Initialize order id array
	for(int i = 0; i < N_customers ; ++i)	{		
		oid[i] = i + 1;
	} 
	
	pthread_t *threads; 							//initializing threads 
	threads = malloc(N_customers*sizeof(pthread_t)); 	//allocates needed memory
	if(threads == NULL)	{
		printf("Couldn't allocate the necessary memory for the threads !\n");
		exit(-1);
		//exits program if memory isnt available for the creation of the threads
	}
	
												
	for( int i=0; i < N_customers; ++i)	{		//creating threads
		run_commands = pthread_create(&threads[i], NULL, &startOrder, &oid[i]); 	//creates threads with Order ID
		int nextorder = rand_r(&seed) % (T_order_high - T_order_low +1) + T_order_low; //random number in given bounds
		sleep(nextorder); 	//waits till next order
		if(run_commands != 0){
			printf("Error while creating threads , error code %d", run_commands);	
			exit(-1);
			} 		//error checker
		}
					
	for( int i=0; i < N_customers; ++i)	{			//joining threads
		run_commands = pthread_join(threads[i], NULL);
		if(run_commands != 0){
			printf("Error while joining threads , error code %d ", run_commands);
			exit(-1);
		}			//error in joining threads  
	}
	
	//after the orders finish prints the needed values 
	//---------------------------------------------------------------------------------
	
	//Print total cost and number of failed and done payments
    printf("\n\nTotal income from pizzas is : %0.2f", Total_pizzas_cost);
    printf("\nNumber of failed payments is : %d", total_failed_payments);
    printf("\nNumber of successful payments is : %d \n", N_customers - total_failed_payments);
    
    //Print average waiting time for cashier
    printf("\n\nAverage waiting time is %f", total_waiting_time / N_customers);
    //Print max waiting time for cashier
    printf("\nMax waiting time is %f", max_waiting_time);
	
	//Print average order service time
    printf("\n\nAverage service time is %f", total_service_time/ N_customers);
    //Print max order service time
    printf("\nMax service time is %f", max_service_time);

    //Print average order freezing time
    printf("\n\nAverage cold time is %f", total_cold_time/N_customers);
    //Print max order freezing time
    printf("\nMax cold time is %f\n\n", max_cold_time);

    //---------------------------------------------------------------------------------

	
	free(threads);		//freeing allocated memory of threads
	return 1; //program is finished 
}
