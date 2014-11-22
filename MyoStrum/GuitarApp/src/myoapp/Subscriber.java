package myoapp;

import java.io.IOException;
import javax.sound.sampled.*;
import redis.clients.jedis.JedisPubSub;

public class Subscriber extends JedisPubSub {

    private ScreenGame screenGame;
    public static Clip clip;

    public Subscriber() {
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    screenGame = new ScreenGame();
                } catch (Exception e) {
                    System.out.println("Subscribing failed. " + e);
                }
            }
        }).start();
    }

    @Override
    public void onMessage(String channel, String message) {
        try {
            clip = (Clip) AudioSystem.getMixer(AudioSystem.getMixerInfo()[0]).getLine(new DataLine.Info(Clip.class, null));
        } catch (LineUnavailableException lue) {
            lue.printStackTrace();
        }
        try {
            clip.open(AudioSystem.getAudioInputStream(getClass().getResource("/media/" + Integer.valueOf(message) + ".wav")));
        } catch (UnsupportedAudioFileException | LineUnavailableException | IOException ex) {
        }
        clip.start();
        do {
            try {
                Thread.sleep(50);
            } catch (InterruptedException ie) {
            }
        } while (clip.isActive());
        //screenGame.addWeapon(Integer.valueOf(message));
        screenGame.drawGame();
    }

    @Override
    public void onPMessage(String pattern, String channel, String message) {
    }

    @Override
    public void onSubscribe(String channel, int subscribedChannels) {
    }

    @Override
    public void onUnsubscribe(String channel, int subscribedChannels) {
    }

    @Override
    public void onPUnsubscribe(String pattern, int subscribedChannels) {
    }

    @Override
    public void onPSubscribe(String pattern, int subscribedChannels) {
    }
}
