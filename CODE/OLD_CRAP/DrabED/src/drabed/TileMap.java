package drabed;

import java.io.*;
import javax.swing.*;
import java.awt.Color;

public class TileMap {
    public static enum MarkerFieldError {NONE, ID_TOO_SHORT, ID_IN_USE, END};

    private Tile[][] mTileMatrix;
    private int mWidth, mHeight;
    private CommandBuffer mCommandBuffer;
    private DefaultComboBoxModel<Marker> mMarkerList;
    TileMap mOldState;
    
    final void reset(int pWidth, int pHeight)
    {
        mWidth = pWidth;
        mHeight = pHeight;
        mTileMatrix = new Tile[pWidth][pHeight];
        
        for(int x = 0; x < pWidth; x++)
        {
            mTileMatrix[x] = new Tile[pHeight];
            for(int y = 0; y < pHeight; y++)
                mTileMatrix[x][y] = new Tile(false);
        } 
        
        mMarkerList = new DefaultComboBoxModel<Marker>();
        mCommandBuffer = new CommandBuffer();
    }
    
    void clear()
    {
        mMarkerList.removeAllElements();
        
        for(int x = 0; x < mWidth; x++)
            for(int y = 0; y < mHeight; y++)
                mTileMatrix[x][y] = new Tile(false);
    }
    
    void copyFrom(TileMap other)
    {
        mMarkerList.removeAllElements();
        for(int i = 0; i < other.mMarkerList.getSize(); i++)
            mMarkerList.addElement(new Marker(other.mMarkerList.getElementAt(i)));
        
        for(int i = 0; i < other.mMarkerList.getSize(); i++)
            other.mMarkerList.getElementAt(i).index(i);
        
        if(other.mWidth != mWidth || other.mHeight != mHeight)
        {
            mWidth = other.mWidth;
            mHeight = other.mHeight;
            mTileMatrix = new Tile[mWidth][mHeight];
            
            for (int x = 0; x < mWidth; x++)
                mTileMatrix[x] = new Tile[mHeight];
        }

        for(int y = 0; y < mHeight; y++)
            for(int x = 0; x < mWidth; x++)
                mTileMatrix[x][y] = new Tile(other.mTileMatrix[x][y], mMarkerList);
    }
    
    void saveState()
    {
        if(mOldState == null)
            mOldState = new TileMap(mWidth, mHeight);
        mOldState.copyFrom(this);
    }
    
    void loadState()
    {
        if(mOldState != null)
            this.copyFrom(mOldState);
        else
            this.clear();
    }
    
    TileMap(int pWidth, int pHeight)
    {
        reset(pWidth, pHeight);
    }
    
    void setCommandBufferListener(CommandBufferListener listener)
    {
        mCommandBuffer.setListener(listener);
    }
    
    boolean undo()
    {
        return mCommandBuffer.undo();
    }
    
    boolean redo()
    {
        return mCommandBuffer.redo();
    }
    
    // **************
    // * Inspectors *
    // **************
    
    int width() { return mWidth; }
    int height() { return mHeight; }
    
    boolean solid(int x, int y)
    {
        return mTileMatrix[x][y].solid();
    }
    
    Color color(int x, int y)
    {
        return mTileMatrix[x][y].color();
    }
    
    ComboBoxModel getComboBoxModel()
    {
        return mMarkerList;
    }
    
    // ************************************
    // * Internal command implementations *
    // ************************************
    
    void action(int x, int y, TileOp op)
    {   
        Tile tile = mTileMatrix[x][y];
        
        if(op.getMode() == TileOp.Mode.SOL)
            switch(op.getSolOp())
            {
                case SET: tile.solid(true); break;
                case CLEAR: tile.solid(false); break;
                case TOGGLE: tile.toggleSolid(); break;
            }
        
        else if(op.getMarkIndex() > -1)
        {
            Marker mark = mMarkerList.getElementAt(op.getMarkIndex());
            switch(op.getEntOp())
            {
                case ADD: tile.occupy(mark); break;
                case REMOVE: tile.evict(mark); break;
                case SET: tile.setOccupant(mark); break;
                case CLEAR: tile.clearOccupantList(); break;
            }
        }
    }

    private void _addMarker(String id, String name, String desc, Color col)
    {
        mMarkerList.addElement(new Marker(id, name, desc, col));
    }
    
    private void _removeMarker(int index)
    {
        for (int y = 0; y < mHeight; y++)
            for (int x = 0; x < mWidth; x++)
                mTileMatrix[x][y].evict(mMarkerList.getElementAt(index));
        mMarkerList.removeElementAt(index);
    }

    private void _set(int x, int y, TileOp op)
    {
        if(x < 0 || x >= mWidth || y < 0 || y >= mHeight) return;
        action(x, y, op);
    }
    
    private void _hline(int x1, int y, int x2, TileOp op)
    {
        int minx, maxx;
        
        if(y < 0 || y >= mHeight) return;
        if(x1 < x2) { minx = x1; maxx = x2; }
        else { minx = x2; maxx = x1; }
        
        minx = minx < 0 ? 0 : minx;
        maxx = maxx > mWidth - 1 ? mWidth - 1 : maxx;
        
        for(int x = minx; x <= maxx; x++)
            action(x, y, op);
    }
    
    // *******************
    // * Command Classes *
    // *******************
    
    private class AddMarkerCmd implements Command {
        private TileMap target;
        private String id, name, desc;
        private Color col;
        
        AddMarkerCmd(TileMap pTarget, String pId, String pName, String pDesc, Color pCol)
        {
            target = pTarget;
            id = pId;
            name = pName;
            desc = pDesc;
            col = pCol;
        }
        
        public void execute()
        {
            target._addMarker(id, name, desc, col);
        }
        
        public void unexecute()
        {
            target.loadState();
            mCommandBuffer.execute();
        }
    }
    
    private class ChangeMarkerCmd implements Command {
        private TileMap target;
        private int index;
        private String id, name, desc;
        private Color col;

        ChangeMarkerCmd(TileMap pTarget, int pIndex, String pId, String pName, String pDesc, Color pCol)
        {
            target = pTarget;
            index = pIndex;
            id = pId;
            name = pName;
            desc = pDesc;
            col = pCol;
        }

        public void execute()
        {
            Marker mark = target.mMarkerList.getElementAt(index);
            
            mark.id(id);
            mark.name(name);
            mark.description(desc);
            mark.color(col);
        }
        
        public void unexecute()
        {
            target.loadState();
            mCommandBuffer.execute();
        }
    }
    
    
    private class RemoveMarkerCmd implements Command {
        private TileMap mTarget;
        private int index;

        RemoveMarkerCmd(TileMap pTarget, int pIndex)
        {
            mTarget = pTarget;
            index = pIndex;
        }

        public void execute()
        {
            mTarget._removeMarker(index);
        }
        
        public void unexecute()
        {
            mTarget.loadState();
            mCommandBuffer.execute();
        }
    }
    
    private class SetCmd implements Command {
        private int mx, my;
        private TileOp mMod;
        private TileMap mTarget;
        
        SetCmd(TileMap pTarget, int px, int py, TileOp pMod)
        {
            mTarget = pTarget;
            mx = px;
            my = py;
            mMod = new TileOp(pMod);
        }

        public void execute()
        {
            mTarget._set(mx, my, mMod);
        }
        
        public void unexecute()
        {
            mTarget.loadState();
            mCommandBuffer.execute();
        }
    }
    
    private class EllipseCmd implements Command {
        private TileMap target;
        private int cx, cy, rx, ry;
        private TileOp op;
        
        EllipseCmd(TileMap pTarget, int pCx, int pCy, int pRx, int pRy, TileOp pMod)
        {
            target = pTarget;
            cx = pCx;
            cy = pCy;
            rx = pRx;
            ry = pRy;
            op = new TileOp(pMod);
        }
        
        public void execute()
        {
            int lastX = 0;
            double scale = (double) rx / (double) ry;

            for (int y = -ry; y <= 0; y++)
            {
                int x = (int) (Math.sqrt(ry * ry - y * y) * scale + 0.5);

                if (x > lastX + 1)
                {
                    target._hline(cx + lastX + 1, cy + y, cx + x, op);
                    target._hline(cx - lastX - 1, cy + y, cx - x, op);

                    target._hline(cx + lastX + 1, cy - y, cx + x, op);
                    target._hline(cx - lastX - 1, cy - y, cx - x, op);
                } else
                {
                    target._set(cx + x, cy + y, op);
                    target._set(cx - x, cy + y, op);

                    target._set(cx + x, cy - y, op);
                    target._set(cx - x, cy - y, op);
                }

                lastX = x;
            }
        }
        
        public void unexecute()
        {
            target.loadState();
            mCommandBuffer.execute();
        }
    }
    
    private class FilledEllipseCmd implements Command {
        private TileMap target;
        private int cx, cy, rx, ry;
        private TileOp op;
        
        FilledEllipseCmd(TileMap pTarget, int pCx, int pCy, int pRx, int pRy, TileOp pMod)
        {
            target = pTarget;
            cx = pCx;
            cy = pCy;
            rx = pRx;
            ry = pRy;
            op = new TileOp(pMod);
        }
        
        public void execute()
        {
            double scale = (double) rx / (double) ry;

            for (int y = -ry; y <= -1; y++)
            {
                int x = (int) (Math.sqrt(ry * ry - y * y) * scale + 0.5);

                target._hline(cx - x, cy + y, cx + x, op);
                target._hline(cx - x, cy - y, cx + x, op);
            }

            int x = (int) (ry * scale + 0.5);
            target._hline(cx - x, cy, cx + x, op);
        }
        
        public void unexecute()
        {
            target.loadState();
            mCommandBuffer.execute();
        }
    }
    
    private class LineCmd implements Command {
        private TileMap target;
        private int x1, y1, x2, y2;
        private TileOp op;
        
        LineCmd(TileMap pTarget, int px1, int py1, int px2, int py2, TileOp pMod)
        {
            target = pTarget;
            x1 = px1;
            y1 = py1;
            x2 = px2;
            y2 = py2;
            op = new TileOp(pMod);
        }

        public void execute()
        {
            int dx = x2 - x1, dy = y2 - y1, miny, maxy;
            int lastX;

            if (dy == 0)
            {
                target._hline(x1, y1, x2, op);
                return;
            }

            if (y1 < y2)
            {
                miny = 0;
                maxy = dy;
            } else
            {
                miny = dy;
                maxy = 0;
            }

            lastX = dx * miny / dy;

            for (int y = miny; y <= maxy; y++)
            {
                int x = dx * y / dy;

                if (x - lastX > 1)
                    target._hline(x1 + lastX + 1, y1 + y, x1 + x, op);
                else if (x - lastX < -1)
                    target._hline(x1 + lastX - 1, y1 + y, x1 + x, op);
                else
                    target._set(x1 + x, y1 + y, op);

                lastX = x;
            }
        }
        
        public void unexecute()
        {
            target.loadState();
            target.mCommandBuffer.execute();
        }
    }
    
    // ******************************
    // * External Command Interface *
    // ******************************
    
    MarkerFieldError addMarker(String id, String name, String desc, Color col)
    {
        if(id.length() < 1) return MarkerFieldError.ID_TOO_SHORT;
        for(int i = 0; i < mMarkerList.getSize(); i++)
            if(id.equals(mMarkerList.getElementAt(i).id()))
                return MarkerFieldError.ID_IN_USE;
        
        AddMarkerCmd cmd = new AddMarkerCmd(this, id, name, desc, col);
        cmd.execute();
        mCommandBuffer.add(cmd);
        return MarkerFieldError.NONE;
    }
    
    MarkerFieldError changeMarker(int index, String id, String name, String desc, Color col)
    {        
        if(id.length() < 1) return MarkerFieldError.ID_TOO_SHORT;
        for(int i = 0; i < mMarkerList.getSize(); i++)
            if(i != index && id.equals(mMarkerList.getElementAt(i).id()))
                return MarkerFieldError.ID_IN_USE;
        
        ChangeMarkerCmd cmd = new ChangeMarkerCmd(this, index, id, name, desc, col);
        cmd.execute();
        mCommandBuffer.add(cmd);
        return MarkerFieldError.NONE;
    }
    
    void removeMarker(int index)
    {
        RemoveMarkerCmd cmd = new RemoveMarkerCmd(this, index);
        cmd.execute();
        mCommandBuffer.add(cmd);
    }
    
    void set(int x, int y, TileOp op)
    {
        SetCmd cmd = new SetCmd(this, x, y, op);
        cmd.execute();
        if(mCommandBuffer != null) mCommandBuffer.add(cmd);
    }
    
    void ellipse(int cx, int cy, int rx, int ry, TileOp op)
    {
        EllipseCmd cmd = new EllipseCmd(this, cx, cy, rx, ry, op);
        cmd.execute();
        if(mCommandBuffer != null) mCommandBuffer.add(cmd);
    }
    
    void filledEllipse(int cx, int cy, int rx, int ry, TileOp op)
    {
        FilledEllipseCmd cmd = new FilledEllipseCmd(this, cx, cy, rx, ry, op);
        cmd.execute();
        if(mCommandBuffer != null) mCommandBuffer.add(cmd);
    }
    
    void line(int x1, int y1, int x2, int y2, TileOp op)
    {
        LineCmd cmd = new LineCmd(this, x1, y1, x2, y2, op);
        cmd.execute();
        if(mCommandBuffer != null) mCommandBuffer.add(cmd);
    }
    
    // ************
    // * File I/O *
    // ************
    
    void write(DataOutputStream out) throws IOException
    {
        out.writeInt(mMarkerList.getSize());

        for (int i = 0; i < mMarkerList.getSize(); i++)
            mMarkerList.getElementAt(i).write(out);
        
        out.writeInt(mWidth);
        out.writeInt(mHeight);
        
        for(int i = 0; i < mMarkerList.getSize(); i++)
            mMarkerList.getElementAt(i).index(i);
        
        for(int y = 0; y < mHeight; y++)
            for(int x = 0; x < mWidth; x++)
                mTileMatrix[x][y].write(out);
    }

    void read(DataInputStream in) throws IOException
    {
        int n = in.readInt();
        mMarkerList.removeAllElements();
        
        for (int i = 0; i < n; i++)
        {
            Marker marker = new Marker("", "", "", Color.BLACK);
            marker.read(in);
            mMarkerList.addElement(marker);
        }
        
        int w = in.readInt();
        int h = in.readInt();
        
        if(w != mWidth || h != mHeight)
            reset(w, h);
        
        for(int y = 0; y < mHeight; y++)
            for(int x = 0; x < mWidth; x++)
                mTileMatrix[x][y].read(in, mMarkerList);
        
        mCommandBuffer.clear();
        saveState();
    }
}
