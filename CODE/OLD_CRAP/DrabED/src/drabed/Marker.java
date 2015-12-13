package drabed;

import java.awt.Color;
import java.io.*;

class Marker {
    private String mId, mName, mDescription;
    private Color mColor;
    private int mIndex;
    
    final void init(String pId, String pName, String pDescription, Color pCol)
    {
        mId = pId;
        mName = pName;
        mDescription = pDescription;
        mColor = pCol;
    }
    
    Marker(String pId, String pName, String pDescription, Color pCol)
    {
        init(pId, pName, pDescription, pCol);
    }
    
    Marker(Marker other)
    {
        mId = other.mId;
        mName = other.mName;
        mDescription = other.mDescription;
        mColor = other.mColor;
    }
    
    public String toString()
    {
        return mId;
    }

    void id(String pId) { mId = pId; }
    void name(String pName) { mName = pName; }
    void description(String pDescription) { mDescription = pDescription;  }
    void color(Color pColor) { mColor = pColor; }
    void index(int i) { mIndex = i; }
    
    String id() { return mId; }
    String name() { return mName; }
    String description() { return mDescription; }
    Color color() { return mColor; }
    int index() { return mIndex; }
    
    private void writeAscii(DataOutputStream out, String str) throws IOException
    {
        out.writeInt(str.length() + 1);
        out.writeBytes(str);
        out.writeByte(0);
    }
    
    private String readAscii(DataInputStream in) throws IOException
    {
        int len = in.readInt() - 1;
        System.out.print(len);
        byte[] arr = new byte[len];
        in.readFully(arr);
        in.readByte();
        return new String(arr);
    }
    
    void write(DataOutputStream out) throws IOException
    {   
        writeAscii(out, mId);
        writeAscii(out, mName);
        writeAscii(out, mDescription);
        out.writeInt(mColor.getRGB());
    }
    
    void read(DataInputStream in) throws IOException
    {
        mId = readAscii(in);
        mName = readAscii(in);
        mDescription = readAscii(in);
        mColor = new Color(in.readInt());
    }
}