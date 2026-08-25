//
// Created by dcend on 30.03.2025.
//

#ifndef FLOWBOUNDEDEDGE_H
#define FLOWBOUNDEDEDGE_H


class FlowBoundedEdge {
public:
    int source;
    int destination;
    int lowerBound;
    int upperBound;
    int currentFlow;
};


#endif //FLOWBOUNDEDEDGE_H
