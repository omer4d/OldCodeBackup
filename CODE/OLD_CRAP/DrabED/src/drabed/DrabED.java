package drabed;

import javax.swing.*;

public class DrabED {

    public static void main(String[] args)
    {
        TileMap tileMap = new TileMap(50, 50);
        JFrame frame = new MainFrame(tileMap);
        frame.setVisible(true);
    }
}
