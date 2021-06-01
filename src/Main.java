public class Main {
    public static void main(String[] args) throws Exception {
        System.out.println(args[5]);
        Gui gui = new Gui(args[1],Integer.parseInt(args[3]),args[5]);
        gui.setVisible(true);
    }
}