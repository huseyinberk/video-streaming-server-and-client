
import java.io.*;
import java.net.Socket;

public class Client extends Thread{
    public Socket socket = null;
    public Gui mainPanel=null;
    public PrintWriter out;
    private  BufferedReader  in=null;
    private String channel="";
    byte[] messageByte = new byte[255];
    public Client(Gui mainPanel)  {
        this.mainPanel=mainPanel;
    }

    @Override
    public void run() {
        int i=0;
        while (true){
            try {
                String packet=in.readLine();
                System.out.println(packet);
                System.out.flush();
                packet=packet.replaceAll(String.valueOf(mainPanel.port)+String.valueOf(mainPanel.port),"\n");
                packet=packet.replaceAll(String.valueOf(mainPanel.port),"\n");
                mainPanel.msgArea.setText(packet);
                mainPanel.msgArea.update(mainPanel.msgArea.getGraphics());
                Thread.sleep(400);
                out.println( channel+ "_" + i);
                i++;

            }catch (IOException | InterruptedException e){
               break;
            }
        }
        close();
    }
    public void setConnectionValues(String ip, int port,String channel){
        try {
            socket=new Socket(ip,port);
            in=new BufferedReader(new InputStreamReader(socket.getInputStream()));
            out=new PrintWriter(socket.getOutputStream(),true);
            this.channel="ch" +channel;
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    public void close(){
        //System.out.println("close");
        try{
            in.close();
            out.close();
            socket.close();
        }catch (Exception e){
            e.printStackTrace();
        }
    }

}
