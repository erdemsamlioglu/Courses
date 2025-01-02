#include <pthread.h>
#include <semaphore.h>
#include <vector>
#include <iostream>
#include <ctime>
#include <tuple>
#include <chrono>
#include "WriteOutput.h"
#include "helper.h"
#include "monitor.h"
#include <queue>



#define MAX_DIRECTION 4





struct Bridge {
    int travelTime;
    int maxWaitTime;
};

struct Car {
    int id;
    int travelTime;
    std::vector<std::tuple<char, int, int, int>> path;
};

struct Ferry {
    int travelTime;
    int maxWaitTime;
    int capacity;
};

struct Crossroad {
    int travelTime;
    int maxWaitTime;
};




std::vector<Crossroad> crossroads;


std::vector<Bridge> narrowBridges;




class CrossroadMonitor : public Monitor {
public:
    int currentDirection;
    int carsOnCrossroad;
    int carsWaiting;
    int waitingCars[MAX_DIRECTION];
    int maxWaitTime;
    int travelTime;
    Condition* readyToPass[MAX_DIRECTION];
    Condition passDelayCondition[4];
    bool timerStarted[MAX_DIRECTION];
    struct timespec abstime[MAX_DIRECTION];
    std::chrono::steady_clock::time_point timerStart[MAX_DIRECTION];
    bool switchedDueToTimeout;
    bool timeout;
    std::queue<int> waitingPassingQueue[MAX_DIRECTION];
    std::queue<int> waitingCrossroadQueue[MAX_DIRECTION];

    CrossroadMonitor(int travelTime, int waitTime)
        : travelTime(travelTime), maxWaitTime(waitTime), passDelayCondition{Condition(this), Condition(this),Condition(this), Condition(this)}, currentDirection(0), carsOnCrossroad(0), carsWaiting(0), switchedDueToTimeout(false),timeout(false) {
        for (int i = 0; i < MAX_DIRECTION; ++i) {
            waitingCars[i] = 0;
            readyToPass[i] = new Condition(this);
            timerStarted[i] = false;
        }
    }

    ~CrossroadMonitor() {
        for (int i = 0; i < MAX_DIRECTION; ++i) {
            delete readyToPass[i];
        }
    }

    void arrive(int carID, int crossroadID, int direction) {
        Lock lock(this);
        WriteOutput(carID, 'C', crossroadID, ARRIVE);

        if (carsOnCrossroad == 0 && waitingCars[currentDirection] == 0) {
            currentDirection = direction;
            readyToPass[direction]->notifyAll();
        }
        
        
        if (currentDirection != direction || (carsOnCrossroad > 0 && direction != currentDirection) || (switchedDueToTimeout && carsOnCrossroad > 0)) {
            waitingCars[direction]++;
            waitingCrossroadQueue[direction].push(carID);
        }
        
        
        
        
        

        while (currentDirection != direction || (carsOnCrossroad > 0 && direction != currentDirection) || (switchedDueToTimeout && carsOnCrossroad > 0) || (!waitingCrossroadQueue[direction].empty() && waitingCrossroadQueue[direction].front() != carID)) {
            if (!timerStarted[direction] && waitingCars[direction] > 0) {
                timerStart[direction] = std::chrono::steady_clock::now();
                timerStarted[direction] = true;
                abstime[direction] = calculate_future_timespec(maxWaitTime);
            }

            int waitResult = 0;
            if (timerStarted[direction] && direction != currentDirection) {
                waitResult = readyToPass[direction]->timedwait(&abstime[direction]);
            } else {
                readyToPass[direction]->wait();
            }

            if (waitResult == ETIMEDOUT && direction != currentDirection) {
            int nextDirection = findNextNonEmptyDirection();

            if (nextDirection != currentDirection) {
                currentDirection = nextDirection;
                resetTimers();
                timeout=true;
                switchedDueToTimeout = true;
            }
            
            
            }
        }
        
        
        
        
        
        waitingPassingQueue[direction].push(carID);
if (!waitingPassingQueue[direction].empty()){
        if (carsOnCrossroad > 0) {
            if (waitingPassingQueue[direction].front() != carID) {
                while (waitingPassingQueue[direction].front() != carID) {
                    struct timespec waitTimeSpec = calculate_future_timespec(PASS_DELAY);
                    passDelayCondition[direction].timedwait(&waitTimeSpec);
                }
                                    struct timespec waitTimeSpec = calculate_future_timespec(PASS_DELAY);
                    passDelayCondition[direction].timedwait(&waitTimeSpec);
                
            } else {
                struct timespec waitTimeSpec = calculate_future_timespec(PASS_DELAY);
                passDelayCondition[direction].timedwait(&waitTimeSpec);
            }
        }
        
        
        
        if(currentDirection!=direction){
        
        
                while (currentDirection != direction || (carsOnCrossroad > 0 && direction != currentDirection) || (switchedDueToTimeout && carsOnCrossroad > 0)) {
            if (!timerStarted[direction] && waitingCars[direction] > 0) {
                timerStart[direction] = std::chrono::steady_clock::now();
                timerStarted[direction] = true;
                abstime[direction] = calculate_future_timespec(maxWaitTime);
            }

            int waitResult = 0;
            if (timerStarted[direction] && direction != currentDirection) {
                waitResult = readyToPass[direction]->timedwait(&abstime[direction]);
            } else {
                readyToPass[direction]->wait();
            }

            if (waitResult == ETIMEDOUT && direction != currentDirection) {
            int nextDirection = findNextNonEmptyDirection();

            if (nextDirection != currentDirection) {
                currentDirection = nextDirection;
                resetTimers();
                timeout=true;
                switchedDueToTimeout = true;
            }
            
            
            }
        }
        
        
        
            if (carsOnCrossroad > 0) {
            if (waitingPassingQueue[direction].front() != carID) {
                while (waitingPassingQueue[direction].front() != carID) {
                    struct timespec waitTimeSpec = calculate_future_timespec(PASS_DELAY);
                    passDelayCondition[direction].timedwait(&waitTimeSpec);
                }
                    struct timespec waitTimeSpec = calculate_future_timespec(PASS_DELAY);
                    passDelayCondition[direction].timedwait(&waitTimeSpec);
                
            } else {
                struct timespec waitTimeSpec = calculate_future_timespec(PASS_DELAY);
                passDelayCondition[direction].timedwait(&waitTimeSpec);
            }
        }
        
        
        
        
        
        
        
        
        }
        

        waitingPassingQueue[direction].pop();
        passDelayCondition[direction].notifyAll();
}











        
        
        if (!waitingCrossroadQueue[direction].empty() && waitingCrossroadQueue[direction].front() == carID) {
            waitingCrossroadQueue[direction].pop();
            waitingCars[direction]--;
            readyToPass[direction]->notifyAll();
        }
        
        
        
  
        carsOnCrossroad++;
        WriteOutput(carID, 'C', crossroadID, START_PASSING);

        switchedDueToTimeout = false;
    }

    void cross(int carID, int crossroadID, int direction) {
        sleep_milli(travelTime);
        {
            Lock lock(this);
            carsOnCrossroad--;
            WriteOutput(carID, 'C', crossroadID, FINISH_PASSING);

            if (carsOnCrossroad == 0) {
            
            if(timeout){
            	timeout=false;
            	readyToPass[currentDirection]->notifyAll();
            	
            	}
            else{
            	switchDirection();
            	}	
            }
        }
    }

private:
    bool shouldWait(int direction) {
        return currentDirection != direction || (carsOnCrossroad > 0 && direction != currentDirection) || (switchedDueToTimeout && carsOnCrossroad > 0);
    }

    void switchDirection() {

            int nextDirection = findNextNonEmptyDirection();
            if (nextDirection != currentDirection) {
                currentDirection = nextDirection;
                resetTimers();
                readyToPass[currentDirection]->notifyAll();
            }
            
            switchedDueToTimeout = false;
        
    }
    

    int findNextNonEmptyDirection() {
        int start = (currentDirection + 1) % MAX_DIRECTION;
        for (int i = 0; i < MAX_DIRECTION; ++i) {
            int dir = (start + i) % MAX_DIRECTION;
            if (waitingCars[dir] > 0) return dir;
        }
        return currentDirection;
    }

    void resetTimers() {
        for (int i = 0; i < MAX_DIRECTION; ++i) {
            if (waitingCars[i] > 0) {
                timerStart[i] = std::chrono::steady_clock::now();
                abstime[i] = calculate_future_timespec(maxWaitTime);
                timerStarted[i] = true;
            }
        }
    }

    struct timespec calculate_future_timespec(int ms) {
        struct timespec ts;
        auto now = std::chrono::system_clock::now();
        auto future = now + std::chrono::milliseconds(ms);
        ts.tv_sec = std::chrono::duration_cast<std::chrono::seconds>(future.time_since_epoch()).count();
        ts.tv_nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(future.time_since_epoch()).count() % 1000000000;
        return ts;
    }

    bool isTimerExpired(int direction) {
        auto now = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - timerStart[direction]).count();
        return elapsedTime >= maxWaitTime;
    }
};

std::vector<CrossroadMonitor> crossroadMonitors;








class FerryMonitor : public Monitor {
public:
    Condition readyToLoad[2];
    Condition readyToDepart[2];
    int currentLoad[2];
    int maxCapacity;
    int travelTime;
    int maxWaitTime;
    bool isDeparting[2];
    bool timerStarted[2];
    struct timespec abstime[2];
    std::chrono::steady_clock::time_point timerStart[2];

FerryMonitor(int cap, int travel, int wait)
        : readyToLoad{Condition(this), Condition(this)}, 
          readyToDepart{Condition(this), Condition(this)}, 
          maxCapacity(cap), travelTime(travel), maxWaitTime(wait) {
        currentLoad[0] = 0;
        currentLoad[1] = 0;
        isDeparting[0] = false;
        isDeparting[1] = false;
        timerStarted[0] = false;
        timerStarted[1] = false;
    }
    
    

    void arrive(int carID, int ferryID, int direction) {
        Lock lock(this);
        WriteOutput(carID, 'F', ferryID, ARRIVE);
        
        
        
        while(isDeparting[direction]){
	readyToLoad[direction].wait();
	}
        

        if (currentLoad[direction] == 0 && !timerStarted[direction]) {
            timerStart[direction] = std::chrono::steady_clock::now();
            timerStarted[direction] = true;
            abstime[direction] = calculate_future_timespec(maxWaitTime);

        }


	
	currentLoad[direction]++;
	
	if (currentLoad[direction] >= maxCapacity || isTimerExpired(direction)) {
            isDeparting[direction] = true;
            readyToDepart[direction].notifyAll();
        }
	

        while (currentLoad[direction] < maxCapacity && !isTimerExpired(direction)) {
            
            int a = readyToDepart[direction].timedwait(&abstime[direction]);
            if (a == ETIMEDOUT || isTimerExpired(direction)) {
                isDeparting[direction] = true;
                readyToDepart[direction].notifyAll();
                break;
            }
            if (a == 0){
                            
                isDeparting[direction] = true;
                break;

            }
        }

    }

    void cross(int carID, int ferryID, int direction) {
        {
            Lock lock(this);
            while (!isDeparting[direction]) {
                readyToDepart[direction].wait();
            }

            
            if (currentLoad[direction]>0) {
            	currentLoad[direction] = currentLoad[direction]-1;

                if(currentLoad[direction]==0){
                isDeparting[direction]=false;
                timerStarted[direction] = false;
                readyToLoad[direction].notifyAll();
                
            }
            }
            
        }

	WriteOutput(carID, 'F', ferryID, START_PASSING);
        sleep_milli(travelTime);
        WriteOutput(carID, 'F', ferryID, FINISH_PASSING);
            

  
    }

    struct timespec calculate_future_timespec(int ms) {
        struct timespec ts;
        auto now = std::chrono::system_clock::now();
        auto future = now + std::chrono::milliseconds(ms);
        ts.tv_sec = std::chrono::duration_cast<std::chrono::seconds>(future.time_since_epoch()).count();
        ts.tv_nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(future.time_since_epoch()).count() % 1000000000;
        return ts;
    }

    bool isTimerExpired(int direction) {
        auto now = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - timerStart[direction]).count();
        return elapsedTime >= maxWaitTime;
    }
};
std::vector<Ferry> ferries;
std::vector<FerryMonitor> ferryMonitors;





class NarrowBridgeMonitor : public Monitor {
public:
    Condition directionChanged[2];
    Condition passDelayCondition[2];
    int currentDirection; 
    int carsOnBridge;    
    int waitingCars[2];
    int maxWaitTime;
    int carsPassing;
    bool timerStarted;
    bool switchedDueToTimeout;
    int carsWaiting;
    struct timespec abstime;
    std::chrono::steady_clock::time_point timerStart;
    std::queue<int> waitingQueue[2];
    std::queue<int> waitingPassingNarrow[2];

    NarrowBridgeMonitor() : directionChanged{Condition(this), Condition(this)}, passDelayCondition{Condition(this), Condition(this)}, currentDirection(0), carsOnBridge(0), maxWaitTime(0), carsPassing(0),carsWaiting(0), timerStarted(false), switchedDueToTimeout(false){
        waitingCars[0] = waitingCars[1] = 0;
    }

    void setMaxWaitTime(int time) {
        maxWaitTime = time;
    }

    void arrive(int carID, int bridgeID, int direction) {
        Lock lock(this);
        WriteOutput(carID, 'N', bridgeID, ARRIVE);


        if (carsOnBridge == 0 && waitingCars[currentDirection] == 0) {
            currentDirection = direction;
            directionChanged[direction].notifyAll();
        }
        
        
        if (currentDirection != direction || (carsOnBridge > 0 && direction != currentDirection) || (switchedDueToTimeout && carsOnBridge > 0)) {
            waitingCars[direction]++;
            waitingQueue[direction].push(carID);
        }
        
        

        while (currentDirection != direction || (carsOnBridge > 0 && direction != currentDirection) || (switchedDueToTimeout && carsOnBridge > 0)|| (!waitingQueue[direction].empty() && waitingQueue[direction].front() != carID)) {
            
            
            if (!timerStarted && waitingCars[1 - currentDirection] > 0) {
            
                timerStart = std::chrono::steady_clock::now();
                timerStarted = true;
                abstime = calculate_future_timespec(maxWaitTime);
            }

            int waitResult = 0;
            if (timerStarted && direction != currentDirection) {
            
                waitResult = directionChanged[direction].timedwait(&abstime);
            } else {
                directionChanged[direction].wait();
            }


            if (waitResult == ETIMEDOUT && direction != currentDirection) {
                currentDirection = direction;
                timerStarted = false;
                switchedDueToTimeout = true;
            }
        }
        
	waitingPassingNarrow[direction].push(carID);
	if (!waitingPassingNarrow[direction].empty()){
		if (carsPassing > 0) {
		    if (waitingPassingNarrow[direction].front() != carID) {
		        while (waitingPassingNarrow[direction].front() != carID) {
		            struct timespec waitTimeSpec = calculate_future_timespec(PASS_DELAY);
		            passDelayCondition[direction].timedwait(&waitTimeSpec);
		        }
		                            struct timespec waitTimeSpec = calculate_future_timespec(PASS_DELAY);
		            passDelayCondition[direction].timedwait(&waitTimeSpec);
		        
		    } else {
		        struct timespec waitTimeSpec = calculate_future_timespec(PASS_DELAY);
		        passDelayCondition[direction].timedwait(&waitTimeSpec);
		    }
		}
		
		
		if(currentDirection!=direction)
		
		
		{        while (currentDirection != direction || (carsOnBridge > 0 && direction != currentDirection) || (switchedDueToTimeout && carsOnBridge > 0)) {
            
            
            if (!timerStarted && waitingCars[1 - currentDirection] > 0) {
            
                timerStart = std::chrono::steady_clock::now();
                timerStarted = true;
                abstime = calculate_future_timespec(maxWaitTime);
            }

            int waitResult = 0;
            if (timerStarted && direction != currentDirection) {
            
                waitResult = directionChanged[direction].timedwait(&abstime);
            } else {
                directionChanged[direction].wait();
            }


            if (waitResult == ETIMEDOUT && direction != currentDirection) {
                currentDirection = direction;
                timerStarted = false;
                switchedDueToTimeout = true;
            }
        }
        		if (carsPassing > 0) {
		    if (waitingPassingNarrow[direction].front() != carID) {
		        while (waitingPassingNarrow[direction].front() != carID) {
		            struct timespec waitTimeSpec = calculate_future_timespec(PASS_DELAY);
		            passDelayCondition[direction].timedwait(&waitTimeSpec);
		        }
		                            struct timespec waitTimeSpec = calculate_future_timespec(PASS_DELAY);
		            passDelayCondition[direction].timedwait(&waitTimeSpec);
		        
		    } else {
		        struct timespec waitTimeSpec = calculate_future_timespec(PASS_DELAY);
		        passDelayCondition[direction].timedwait(&waitTimeSpec);
		    }
		}
        
        
        
		
		
		
		
		
		
		
		}
			
		waitingPassingNarrow[direction].pop();
		passDelayCondition[direction].notifyAll();
	}








if (!waitingQueue[direction].empty() && waitingQueue[direction].front() == carID) {
            waitingQueue[direction].pop();
            waitingCars[direction]--;
            directionChanged[direction].notifyAll();
        }




        carsOnBridge++;
        carsPassing++;
        WriteOutput(carID, 'N', bridgeID, START_PASSING);

        switchedDueToTimeout = false;
        

        
        
    }

    void cross(int carID, int bridgeID, int direction) {
        sleep_milli(narrowBridges[bridgeID].travelTime);

        Lock lock(this);
        carsOnBridge--;
        carsPassing--;

        WriteOutput(carID, 'N', bridgeID, FINISH_PASSING);

        if (carsOnBridge == 0) {
            if (waitingCars[1 - direction] > 0) {
                currentDirection = 1 - direction;
                timerStarted = false;
            }
            directionChanged[currentDirection].notifyAll();
        }
    }

private:
    struct timespec calculate_future_timespec(int ms) {
        struct timespec ts;
        auto now = std::chrono::system_clock::now();
        auto future = now + std::chrono::milliseconds(ms);
        ts.tv_sec = std::chrono::duration_cast<std::chrono::seconds>(future.time_since_epoch()).count();
        ts.tv_nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(future.time_since_epoch()).count() % 1000000000;
        return ts;
    }

    bool isTimerExpired() {
        auto now = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - timerStart).count();
        return elapsedTime >= maxWaitTime;
    }
};





std::vector<NarrowBridgeMonitor> bridgeMonitors;


void* carJourney(void* arg) {
    Car* car = (Car*)arg;
    for (auto& step : car->path) {
        char connectorType = std::get<0>(step);
        int bridgeID = std::get<1>(step);
        int fromDirection = std::get<2>(step);
        int toDirection = std::get<3>(step); 
        
        WriteOutput(car->id, connectorType, bridgeID, TRAVEL);
        sleep_milli(car->travelTime);

        if (connectorType == 'N') {
            NarrowBridgeMonitor& monitor = bridgeMonitors[bridgeID];
            monitor.arrive(car->id, bridgeID, fromDirection);
            monitor.cross(car->id, bridgeID, fromDirection);
        }
        if (connectorType == 'F') {
        FerryMonitor& monitor = ferryMonitors[bridgeID];
        monitor.arrive(car->id, bridgeID, fromDirection);
        monitor.cross(car->id, bridgeID, fromDirection);
    	}
    	
    	if (connectorType == 'C') {
                CrossroadMonitor& monitor = crossroadMonitors[bridgeID];
                
                monitor.arrive(car->id, bridgeID, fromDirection);
                monitor.cross(car->id, bridgeID, fromDirection);
    	}
    	

    }
    return NULL;
}






int main() {
    

    int numNarrowBridges, numFerries, numCrossroads, numCars;
    
    
    std::cin >> numNarrowBridges;
    narrowBridges.resize(numNarrowBridges);
    bridgeMonitors.resize(numNarrowBridges);

    for (int i = 0; i < numNarrowBridges; ++i) {
        int travelTime, maxWaitTime;
        std::cin >> travelTime >> maxWaitTime;
        narrowBridges[i].travelTime = travelTime;
        narrowBridges[i].maxWaitTime = maxWaitTime;
        bridgeMonitors[i].setMaxWaitTime(maxWaitTime);
    }
    
std::cin >> numFerries;
    ferries.resize(numFerries);
    ferryMonitors.reserve(numFerries);  

    for (int i = 0; i < numFerries; ++i) {
        int travelTime, maxWaitTime, capacity;
        std::cin >> travelTime >> maxWaitTime >> capacity;
        ferries[i] = {travelTime, maxWaitTime, capacity};
        ferryMonitors.emplace_back(capacity, travelTime, maxWaitTime);
    }
	
	

    std::cin >> numCrossroads;
    crossroads.resize(numCrossroads);
    crossroadMonitors.reserve(numCrossroads);
    for (int i = 0; i < numCrossroads; ++i) {
    	int travelTime, maxWaitTime;
    	std::cin >> travelTime >> maxWaitTime;
    	crossroads[i] = {travelTime, maxWaitTime};
        crossroadMonitors.emplace_back(travelTime, maxWaitTime);
    }
    
    

    

    std::cin >> numCars;
    std::vector<Car> cars(numCars);
    InitWriteOutput();
    std::vector<pthread_t> carThreads(numCars);

    for (int i = 0; i < numCars; ++i) {
        cars[i].id = i;
        int travelTime, pathLength;
        std::cin >> travelTime >> pathLength;
        cars[i].travelTime = travelTime;
        cars[i].path.reserve(pathLength);

        for (int j = 0; j < pathLength; ++j) {
            char connectorType;
            int bridgeID, from, to;
            std::cin >> connectorType >> bridgeID >> from >> to;
            cars[i].path.emplace_back(connectorType, bridgeID, from, to);
        }

        pthread_create(&carThreads[i], NULL, carJourney, &cars[i]);
    }

    for (auto& thread : carThreads) {
        pthread_join(thread, NULL);
    }

    return 0;
}
