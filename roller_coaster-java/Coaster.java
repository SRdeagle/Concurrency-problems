import java.util.concurrent.Semaphore;
import java.util.Queue;
import java.util.LinkedList;


public class Coaster implements Runnable {
    private int numRides;
    private int rideCapacity;
    public Queue<Rider> waitingQueue = new LinkedList<>();
    public Queue<Wagon> wagons = new LinkedList<>();
    public Semaphore boardingMutex = new Semaphore(1);
    public Semaphore coasterSem = new Semaphore(0);
    public Semaphore wagonMutex = new Semaphore(1);
    public Semaphore wagonWait = new Semaphore(1);

    public Coaster(int rideCapacity, int numRides) {
        this.rideCapacity = rideCapacity;
        this.numRides = numRides;
    }
    public int getNumRides() {
        return numRides;
    }
    public int getRideCapacity() {
        return rideCapacity;
    }

    public void run() {
        while (true) {
            try {
                coasterSem.acquire();
                System.out.println("Coaster is starting a ride.");
                // Search for a free wagon
                
                System.out.println("Searching for a free wagon");
                
                wagonWait.acquire(); // Wait until a wagon is available
                wagonMutex.acquire();
                Wagon wagon = wagons.poll();
                if (wagon == null) {
                    System.out.println("!!! No wagons available !!!");
                    return; // Fatal error, should never happen
                }
                if (!wagons.isEmpty()) {
                    System.out.println("Found one, no more available");
                    wagonWait.release();
                }
                System.out.println("Found a free wagon");
                wagonMutex.release();
                boardingMutex.acquire(); // Allow riders to board the wagon
                for (int i = 0; i < rideCapacity; i++) {
                    Rider rider = waitingQueue.poll();
                    if (rider != null) {
                        wagon.board(rider);
                        rider.boardSem.release();
                    }
                }
                System.out.println("All riders boarded, starting the ride");
                wagon.wagonSem.release(); // Signal the wagon to start the ride
                boardingMutex.release();

            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    

}
