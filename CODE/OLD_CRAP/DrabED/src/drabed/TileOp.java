/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package drabed;

/**
 *
 * @author Stas
 */
class TileOp {
    public static enum MarkOp {ADD, REMOVE, SET, CLEAR}
    public static enum SolOp {SET, CLEAR, TOGGLE}
    public static enum Mode {SOL, MARK}
    
    private MarkOp mMarkOp;
    private SolOp mSolOp;
    private Mode mMode;
    private int mMarkIndex;
    
    TileOp()
    {
        mMarkOp = MarkOp.ADD;
        mSolOp = SolOp.SET;
        mMode = Mode.SOL;
        mMarkIndex = -1;
    }
    
    TileOp(TileOp other)
    {
        mMarkOp = other.mMarkOp;
        mSolOp = other.mSolOp;
        mMode = other.mMode;
        mMarkIndex = other.mMarkIndex;
    }
    
    MarkOp getEntOp() { return mMarkOp; }
    void setEntOp(MarkOp op) { mMarkOp = op; }
    
    SolOp getSolOp() { return mSolOp; }
    void setSolOp(SolOp op) { mSolOp = op; }
    
    Mode getMode() { return mMode; }
    void setMode(Mode mode) { mMode = mode; }
    
    int getMarkIndex() { return mMarkIndex; }
    void setMarkIndex(int index) { mMarkIndex = index; }
}