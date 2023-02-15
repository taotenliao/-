package com.example.eightynumberquestion.Controller;

import com.fasterxml.jackson.annotation.JsonProperty;

public class PostObj {
    @JsonProperty("row")
    int row;
    @JsonProperty("cl")
    int cl;
    @JsonProperty("layer")
    int layer;
    @JsonProperty("source")
    String source;
    @JsonProperty("dest")
    String dest;

    public int getRow() {
        return row;
    }

    public void setRow(int row) {
        this.row = row;
    }

    public int getCl() {
        return cl;
    }

    public void setCl(int cl) {
        this.cl = cl;
    }

    public int getLayer() {
        return layer;
    }

    public void setLayer(int layer) {
        this.layer = layer;
    }

    public String getSource() {
        return source;
    }

    public void setSource(String source) {
        this.source = source;
    }

    public String getDest() {
        return dest;
    }

    public void setDest(String dest) {
        this.dest = dest;
    }
}
