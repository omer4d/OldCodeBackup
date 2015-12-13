package drabed;

import java.util.LinkedList;
import java.util.Iterator;
import java.awt.Color;
import java.io.*;
import javax.swing.*;

class Tile {
    private LinkedList mOccupantList;
    private boolean mSolid;
    
    Tile(boolean pSolid)
    {
        mOccupantList = null;
        mSolid = pSolid;
    }
    
    Tile(Tile other, DefaultComboBoxModel<Marker> otherMarkerList)
    {
        mSolid = other.mSolid;
        
        if(other.mOccupantList != null)
        {
            Iterator itr = other.mOccupantList.iterator();

            while(itr.hasNext())
            {
                Marker marker = (Marker)itr.next();
                occupy(otherMarkerList.getElementAt(marker.index()));
            }
        }
    }
    
    boolean solid() { return mSolid; }
    void solid(boolean pSolid) { mSolid = pSolid; }
    
    void toggleSolid() { mSolid = !mSolid; }
    
    final void occupy(Marker mark)
    {
        if(mark == null) return;
        if(mOccupantList == null) mOccupantList = new LinkedList();
        if(!mOccupantList.contains(mark))
            mOccupantList.add(mark);
    }
    
    void evict(Marker mark)
    {
        if(mark == null || mOccupantList == null) return;
        mOccupantList.remove(mark);
        if(mOccupantList.isEmpty()) mOccupantList = null;
    }
    
    void clearOccupantList()
    {
        mOccupantList = null;
    }
    
    void setOccupant(Marker mark)
    {
        if(mOccupantList == null) return;
        if(mark == null)
        {
            clearOccupantList();
            return;
        }
            
        mOccupantList.clear();
        mOccupantList.add(mark);
    }
    
    Color color()
    {
        if(mOccupantList == null)
            return mSolid ? new Color(70, 70, 70) : new Color(240, 240, 240);
            
        int r = 0, g = 0, b = 0, c = 0;
        
        Iterator itr = mOccupantList.iterator();
        
        while (itr.hasNext())
        {
            Color col = ((Marker)itr.next()).color();
            r += col.getRed();
            g += col.getGreen();
            b += col.getBlue();
            c++;
        }
        
        r /= c;
        g /= c;
        b /= c;
        
        if(mSolid) return new Color((int)(r * 0.3), (int)(g * 0.3), (int)(b * 0.3));
        else return new Color(r, g, b);
    }
    
    void write(DataOutputStream out) throws IOException
    {
        if(mOccupantList == null)
            out.writeInt(0);
        
        else
        {
            Iterator itr = mOccupantList.iterator();
            out.writeInt(mOccupantList.size());

            while (itr.hasNext())
                out.writeInt(((Marker) itr.next()).index());
        }
        
        out.writeBoolean(mSolid);
    }
    
    void read(DataInputStream in, DefaultComboBoxModel<Marker> markerList) throws IOException
    {
        int n = in.readInt();
        mOccupantList = null;
        
        if(n > 0)
        {
            mOccupantList = new LinkedList();
            for (int i = 0; i < n; i++)
                mOccupantList.add(markerList.getElementAt(in.readInt()));
        }
        
        mSolid = in.readBoolean();
    }
}