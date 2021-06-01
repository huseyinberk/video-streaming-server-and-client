
public class Sclient {
    public static void main(String[] args) throws Exception {
        String ip="",channel="";
        int port=0;
        for(int i=0;i<5;i+=2){
            switch (args[i]){
                case "-a":
                    ip=args[i+1];
                    break;
                case "-p":
                    port=Integer.parseInt(args[i+1]);
                    break;
                case "-ch":
                    channel=args[i+1];
                    break;
            }
        }
        System.out.println(ip+port+channel);
        Gui gui = new Gui(ip,port,channel);
        gui.setVisible(true);
    }
}