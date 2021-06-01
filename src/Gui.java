

import javax.swing.*;

import java.awt.*;
import java.awt.event.*;

import java.nio.charset.StandardCharsets;


public class Gui  extends JFrame  {
    private JPanel rootPanel;
    private JButton connectButton;
    private JButton disconnectButton;
    private JPanel messagingArea;
    private JPanel generalControl;
    private JButton sendButton;
    private JLabel connectionStatusLabel;
    private JScrollPane messageStream;
    public JTextArea msgArea;
    private boolean connection=false;
    private Client client=null;
    private Packet packet;
private String ip,channel;
public int port;
    public Gui(String ip, int port, String channel) {
        this.ip=ip;
        this.port=port;
        this.channel=channel;
        setup();

        connectButton.addActionListener(
                this::setConnectButton
        );
        disconnectButton.addActionListener(
                this::setDisconnectButton
        );


    }

    private void closeWindow(){
        if(client!=null)
         //   this.client.closeConnection();
        this.packet=null;
        this.client=null;
    }

    public void addTextToMsgArea(String msg){
        this.msgArea.insert(msg+"\n",this.msgArea.getText().length());
    }

    public void setDisconnectButton(ActionEvent e){
        connectionStatusLabel.setText("Not Connected");
        //addTextToMsgArea(username + " disconnected.");
        connection=false;
        activateButtons();
        if(client!=null)
            client.close();
        client=null;

    }
    public void setConnectButton( ActionEvent e){
                connectionStatusLabel.setText("Connected");
                this.client=new Client(this);
        client.setConnectionValues(ip,port,channel);
                connection=true;
                activateButtons();
                this.client.start();
    }
    public void activateButtons(){
        disconnectButton.setEnabled(connection);
        connectButton.setEnabled(!connection);
    }

    public void setup(){
        connectionStatusLabel.setText("Not Connected");
        msgArea.setEditable(false);
        msgArea.setFont(new Font("Monospaced", Font.PLAIN, 12));
        activateButtons();
        add(rootPanel);
        setTitle("Crypto Messenger");
        setSize(840,950);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        msgArea.setLineWrap(true);
        msgArea.setWrapStyleWord(true);

        addWindowListener(new WindowAdapter() {
            @Override
            public void windowClosing(WindowEvent e) {
                super.windowClosing(e);
                closeWindow();
            }

        });
    }



}