import java.util.concurrent.Semaphore;
import java.util.LinkedList;
import java.util.List;

public class Wagon implements Runnable {
    private Coaster coaster;
    public Semaphore wagonSem = new Semaphore(0);
    private List<Rider> riders = new LinkedList<>();
    private long wait; // Maximum time the wagon waits before starting the ride (ms)

    public Wagon(Coaster coaster, long wait) {
        this.coaster = coaster;
        this.wait = wait;
    }

    private void ride() {
        System.out.println("Wagon is riding with " + riders.size() + " riders.");
        try {
            Thread.sleep((long) (Math.random() * wait)); // Simulate the time taken for the ride
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
    public void board(Rider r) throws InterruptedException {
        riders.add(r);
    }

    public void run() {
        while (true) {
            try {
                wagonSem.acquire(); // Wait for the coaster to signal that it's time to ride

                ride(); // Simulate the ride

                for (Rider rider : riders) { // Let every rider know that the ride is over
                    rider.boardSem.release();
                    
                }

                for (Rider rider : riders) { // Wait for each rider to unboard
                    rider.unboardSem.acquire();
                    
                }

                riders.clear(); // Clear the list of riders for the next round

                coaster.wagonMutex.acquire();
                if (coaster.wagons.isEmpty()) {
                    coaster.wagons.add(this); // Add this wagon back to the coaster's queue of available wagons
                    coaster.wagonWait.release(); // Signal the coaster that a wagon is available
                } else {
                    coaster.wagons.add(this); // Add this wagon back to the coaster's queue of available wagons
                }
                coaster.wagonMutex.release();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}
