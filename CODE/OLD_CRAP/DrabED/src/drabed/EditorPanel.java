package drabed;

import javax.swing.*;
import java.awt.*;
import java.awt.geom.*;
import java.awt.event.*;
import java.awt.Dimension;

class EditorPanel extends JPanel implements MouseListener, MouseMotionListener {
    public enum Tool {PEN, LINE, ELLIPSE, FILLED_ELLIPSE }
    
    private TileMap mTileMap;
    private TileOp mTileOp;
    private float mTileSize, mScrollX, mScrollY;
    private Tool mTool;
    
    private int mSrcX, mSrcY, mDestX, mDestY, mLastX, mLastY;
    private boolean mDragged;
    
    public EditorPanel(TileMap pTileMap)
    {
        mTileMap = pTileMap;
        mTileOp = new TileOp();
        mTileSize = 20.0f;
        mScrollX = 0.0f;
        mScrollY = 0.0f;
        
        mTool = Tool.PEN;
        mDragged = false;
        
        setFocusable(true);
        adjustSize();
        addMouseListener(this);
        addMouseMotionListener(this);
        setCursor(new Cursor(Cursor.CROSSHAIR_CURSOR));
    }
    
    void setTool(Tool t) { mTool = t; }
    TileOp tileOp() { return mTileOp; }
    
    final void adjustSize()
    {
        setPreferredSize(new Dimension((int)(mTileMap.width() * mTileSize), (int)(mTileMap.height() * mTileSize)));
    }
    
    float g2sx(int gx) { return mTileSize * gx + mScrollX; }
    float g2sy(int gy) { return mTileSize * gy + mScrollY; }
    
    int s2gx(int sx) { return (int)((sx - mScrollX) / mTileSize); }
    int s2gy(int sy) { return (int)((sy - mScrollY) / mTileSize); }
    
    int clamp(int n, int min, int max)
    {
        return n > max ? max : (n < min ? min : n);
    }
    
    public void paintComponent(Graphics g)
    {
        super.paintComponent(g);
        Graphics2D g2d = (Graphics2D)g;
        Rectangle2D r = new Rectangle2D.Double();
        Color lightGrey = new Color(200, 200, 200);
        Color darkGrey = new Color(100, 100, 100);
        
        int minx = clamp(s2gx(0), 0, mTileMap.width()),
            maxx = clamp(s2gx(800), 0, mTileMap.width()),
            miny = clamp(s2gy(0), 0, mTileMap.height()),
            maxy = clamp(s2gy(800), 0, mTileMap.height());

        for(int y = 0; y < mTileMap.height(); y++)
            for(int x = 0; x < mTileMap.width(); x++)
            {
                r.setFrame(g2sx(x), g2sy(y), mTileSize, mTileSize);
                
                g2d.setColor(mTileMap.color(x, y));
                g2d.fill(r);
                
                g2d.setColor(lightGrey);
                g2d.draw(r);
            }
        
        if(mDragged)
            switch(mTool)
            {
                case LINE:
                    float offs = mTileSize * 0.5f;
                    Line2D line = new Line2D.Float(g2sx(mSrcX) + offs, g2sy(mSrcY) + offs,
                                                   g2sx(mDestX) + offs, g2sy(mDestY) + offs);
                    g2d.setColor(darkGrey);
                    g2d.draw(line);
                break;
                    
                case ELLIPSE:
                case FILLED_ELLIPSE:
                   float rx = Math.abs(g2sx(mDestX) - g2sx(mSrcX)),
                         ry = Math.abs(g2sy(mDestY) - g2sy(mSrcY));
                   Ellipse2D ellipse = new Ellipse2D.Float(mTileSize * 0.5f + g2sx(mSrcX) - rx,
                                                           mTileSize * 0.5f + g2sy(mSrcY) - ry, 2 * rx, 2 * ry);
                   g2d.setColor(darkGrey);
                   g2d.draw(ellipse); 
                break;
            }
    }
    
    public void mousePressed(MouseEvent event)
    {
        mSrcX = s2gx(event.getX());
        mSrcY = s2gy(event.getY());
        mDragged = true;
        
        if(mTool == Tool.PEN)
        {
            mTileMap.set(mSrcX, mSrcY, mTileOp);
            repaint();
        }
        
        mLastX = mDestX;
        mLastY = mDestY;
    }
    
    public void mouseDragged(MouseEvent event)
    {
        mDestX = s2gx(event.getX());
        mDestY = s2gy(event.getY());
             
        if(mTool == Tool.PEN && (mDestX != mLastX || mDestY != mLastY))
            mTileMap.set(mDestX, mDestY, mTileOp);
        
        repaint();
        mLastX = mDestX;
        mLastY = mDestY;
    }

    public void mouseReleased(MouseEvent event)
    {
        mDragged = false;
        mDestX = s2gx(event.getX());
        mDestY = s2gy(event.getY());
        
        switch(mTool)
        {
            case LINE:
                mTileMap.line(mSrcX, mSrcY, mDestX, mDestY, mTileOp);
            break;

            case ELLIPSE:
                mTileMap.ellipse(mSrcX, mSrcY, Math.abs(mDestX - mSrcX), Math.abs(mDestY - mSrcY), mTileOp);
            break;
                
            case FILLED_ELLIPSE:
                mTileMap.filledEllipse(mSrcX, mSrcY, Math.abs(mDestX - mSrcX), Math.abs(mDestY - mSrcY), mTileOp);
            break;
        }
        
        repaint();
    }
    
    public void mouseMoved(MouseEvent event) {}
    public void mouseExited(MouseEvent event){}
    public void mouseEntered(MouseEvent event){}
    public void mouseClicked(MouseEvent event){}
}
