import java.util.concurrent.Semaphore;

public class Rider implements Runnable {
    private Coaster coaster;
    private long wait;
    public Semaphore boardSem = new Semaphore(0);
    public Semaphore unboardSem = new Semaphore(0);


    public Rider(Coaster coaster, long wait) {
        this.coaster = coaster;
        this.wait = wait;
    }

    private void board() throws InterruptedException {

        coaster.boardingMutex.acquire();
        coaster.waitingQueue.add(this);
        if (coaster.waitingQueue.size() % coaster.getRideCapacity() == 0) {
            coaster.coasterSem.release();
        }
        coaster.boardingMutex.release();
        boardSem.acquire();
    }

    private void ride() throws InterruptedException {
        // Rider is now on the ride, waiting for it to finish
        boardSem.acquire();
    }

    private void unboard() throws InterruptedException {
        unboardSem.release(); // Let the wagon know that this rider has unboarded
    }

    public void run() {
        while (true) {
            try {
                Thread.sleep((long) (Math.random() * wait));
                board();
                ride();
                unboard();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}
