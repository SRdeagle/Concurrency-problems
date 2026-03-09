 // The Roller Coaster problem, with an arbitrary number of rides and riders, FIFO using semaphores and a coordinator process (the coaster)
 // with private semaphores for cleaner synchronization (no stampedes, no busy waiting, no deadlocks, no starvation)
 
 public class Main {
    public static void main(String[] args) {
        int numWagons = 3; // Number of wagons available
        int rideCapacity = 4; // Capacity of each ride
        int numRiders = 16; // Total number of riders
        int riderWaitTime = 200; // Maximum time a rider waits before trying to board again (ms)
        int wagonWaitTime = 300; // Maximum time the wagon waits before starting the ride (ms)

        Coaster coaster = new Coaster(rideCapacity, numWagons);
        Thread coasterThread = new Thread(coaster);
        for (int i = 0; i < numWagons; i++) {
            Wagon wagon = new Wagon(coaster, wagonWaitTime);
            coaster.wagons.add(wagon);
            Thread wagonThread = new Thread(wagon);
            wagonThread.start();
        }
        coasterThread.start();

        for (int i = 0; i < numRiders; i++) {
            Rider rider = new Rider(coaster, riderWaitTime);
            Thread riderThread = new Thread(rider);
            riderThread.start();
        }
    }
}
