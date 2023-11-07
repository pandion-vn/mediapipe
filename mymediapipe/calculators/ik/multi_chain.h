#ifndef MULTI_CHAIN_H
#define MULTI_CHAIN_H

#include <stdio.h>
#include <vector>
#include <stack>
#include <map>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"

#include "chain.h"

struct ChainNode {
    Chain *value;
    ChainNode *parent;
    std::vector<ChainNode*> *children;
};

class MultiChain {
public:
    MultiChain(std::vector<Chain*> chains);
    bool Insert(ChainNode *root, Chain *chain);
    void Solve();
    void Render(glm::mat4 view, glm::mat4 proj);

    ChainNode *root;
    std::map<ChainNode*, bool> leaves;
    glm::vec3 origin;
  
private:
    void Forward(ChainNode *root);
    void Backward(ChainNode *root);
};

#endif