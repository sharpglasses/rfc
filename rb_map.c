#include "rb_map.h"

/**
 * @brief rb_map copy from july
 * @file rb_map.c
 * @author july
 * @date 03/11/2014
 * @note change from july's blog from csdn 
 *      1.mem cost much
 *      2.good at search
 *
 */


static rb_node_t *rb_new_node(rb_map_t *rbp, rb_key_t key, rb_data_t data);
static rb_node_t* rb_rotate_left(rb_node_t* node, rb_node_t* root);
static rb_node_t* rb_rotate_right(rb_node_t* node, rb_node_t* root);
static rb_node_t* rbp_search_auxiliary(rb_key_t key, rb_node_t* root, rb_node_t** save);
static rb_node_t * rbp_set_rebalance(rb_node_t *node, rb_node_t *root);
static rb_node_t* rbp_erase_rebalance(rb_node_t *node, rb_node_t *parent, rb_node_t *root);


rb_map_t * alloc_rbp(void * (*rb_alloc)(u_int32_t), void (*rb_destory)(void *)){
        rb_map_t *rbp = (struct rb_map*)rb_alloc(sizeof(struct rb_map));
        if(!rbp){
            return 0;
        }
        rbp->rb_alloc   = rb_alloc;
        rbp->rb_destory = rb_destory;
        rbp->root       = 0;
        return rbp;
}

void destory_rbp(rb_map_t *rbp){
        while(rbp->root){
            rbp_erase(rbp, rbp->root->key);
        }
        rbp->rb_destory(rbp);
}

static rb_node_t *rb_new_node(rb_map_t * rbp, rb_key_t key, rb_data_t data)
{
    rb_node_t *node = (rb_node_t *)rbp->rb_alloc(sizeof(struct rb_node));
    if(!node){
        fprintf(stderr, "malloc error\n");   
        return 0;
    }
    node->key = key;
    node->data = data;
    return node;
}


//一、左旋代码分析
/*-----------------------------------------------------------
|   node           right
|   / \    ==>     / \
|   a  right     node  y
|       / \       / \
|       b  y     a   b    //左旋
-----------------------------------------------------------*/
static rb_node_t* rb_rotate_left(rb_node_t* node, rb_node_t* root)
{
    rb_node_t* right = node->right;    //指定指针指向 right<--node->right

    if ((node->right = right->left))
    {
        right->left->parent = node;  //好比上面的注释图，node成为b的父母
    }
    right->left = node;   //node成为right的左孩子

    if ((right->parent = node->parent))
    {
        if (node == node->parent->right)
        {
            node->parent->right = right;
        }
        else
        {
            node->parent->left = right;
        }
    }
    else
    {
        root = right;
    }
    node->parent = right;  //right成为node的父母

    return root;
}


//二、右旋
/*-----------------------------------------------------------
|       node            left
|       / \             / \
|    left  y   ==>    a    node
|   / \                    / \
|  a   b                  b   y  //右旋与左旋差不多，分析略过
-----------------------------------------------------------*/
static rb_node_t* rb_rotate_right(rb_node_t* node, rb_node_t* root)
{
    rb_node_t* left = node->left;

    if ((node->left = left->right))
    {
        left->right->parent = node;
    }
    left->right = node;

    if ((left->parent = node->parent))
    {
        if (node == node->parent->right)
        {
            node->parent->right = left;
        }
        else
        {
            node->parent->left = left;
        }
    }
    else
    {
        root = left;
    }
    node->parent = left;

    return root;
}


//三、红黑树查找结点
//----------------------------------------------------
//rbp_search_auxiliary：查找
//rb_node_t* rbp_search：返回找到的结点
//----------------------------------------------------
static rb_node_t* rbp_search_auxiliary(rb_key_t key, rb_node_t* root, rb_node_t** save)
{
    rb_node_t *node = root, *parent = NULL;

    while (node)
    {
        parent = node;
        if (node->key > key)
        {
            node = node->left;
        }
        else if (node->key < key)
        {
            node = node->right;
        }
        else
        {
            return node;
        }
    }

    if (save)
    {
        *save = parent;
    }

    return NULL;
}

//返回上述rbp_search_auxiliary查找结果
rb_node_t* rbp_search(rb_map_t *rbp, rb_key_t key)
{
    return rbp_search_auxiliary(key, rbp->root, NULL);
}


//四、红黑树的插入
//---------------------------------------------------------
//红黑树的插入结点
s_int32_t rbp_set(rb_map_t *rbp, rb_key_t key, rb_data_t data)
{
    rb_node_t *parent = NULL, *node;

    parent = NULL;
    if ((node = rbp_search_auxiliary(key, rbp->root, &parent)))  //调用rbp_search_auxiliary找到插入结点的地方

    {
        node->data = data;
        return 0;
    }

    node = rb_new_node(rbp, key, data);  //分配结点
    node->data = data;
    if(!node){
        return -1;
    }
    node->parent = parent;
    node->left = node->right = NULL;
    node->color = RED;

    if (parent)
    {
        if (parent->key > key)
        {
            parent->left = node;
        }
        else
        {
            parent->right = node;
        }
    }
    else
    {
        rbp->root = node;
    }
    rbp->root = rbp_set_rebalance(node, rbp->root);   //插入结点后，调用rbp_set_rebalance修复红黑树的性质
    return 0;
}


//五、红黑树的3种插入情况
//接下来，咱们重点分析针对红黑树插入的3种情况，而进行的修复工作。
//--------------------------------------------------------------
//红黑树修复插入的3种情况
//为了在下面的注释中表示方便，也为了让下述代码与我的倆篇文章相对应，
//用z表示当前结点，p[z]表示父母、p[p[z]]表示祖父、y表示叔叔。
//--------------------------------------------------------------
static rb_node_t * rbp_set_rebalance(rb_node_t *node, rb_node_t *root)
{
    rb_node_t *parent, *gparent, *uncle, *tmp;  //父母p[z]、祖父p[p[z]]、叔叔y、临时结点*tmp

    while ((parent = node->parent) && parent->color == RED)
    {     //parent 为node的父母，且当父母的颜色为红时
        gparent = parent->parent;   //gparent为祖父

        if (parent == gparent->left)  //当祖父的左孩子即为父母时。
                                 //其实上述几行语句，无非就是理顺孩子、父母、祖父的关系。:D。
        {
            uncle = gparent->right;  //定义叔叔的概念，叔叔y就是父母的右孩子。

            if (uncle && uncle->color == RED) //情况1：z的叔叔y是红色的
            {
                uncle->color = BLACK;   //将叔叔结点y着为黑色
                parent->color = BLACK;  //z的父母p[z]也着为黑色。解决z，p[z]都是红色的问题。
                gparent->color = RED;
                node = gparent;     //将祖父当做新增结点z，指针z上移俩层，且着为红色。
    //上述情况1中，只考虑了z作为父母的右孩子的情况。
            }
            else                     //情况2：z的叔叔y是黑色的，
            {
                if (parent->right == node)  //且z为右孩子
                {
                    root = rb_rotate_left(parent, root); //左旋[结点z，与父母结点]
                    tmp = parent;
                    parent = node;
                    node = tmp;     //parent与node 互换角色
                }
                             //情况3：z的叔叔y是黑色的，此时z成为了左孩子。
                                    //注意，1：情况3是由上述情况2变化而来的。
                                    //......2：z的叔叔总是黑色的，否则就是情况1了。
                parent->color = BLACK;   //z的父母p[z]着为黑色
                gparent->color = RED;    //原祖父结点着为红色
                root = rb_rotate_right(gparent, root); //右旋[结点z，与祖父结点]
            }
        }

        else
        {
   //这部分是特别为情况1中，z作为左孩子情况，而写的。
            uncle = gparent->left;  //祖父的左孩子作为叔叔结点。[原理还是与上部分一样的]
            if (uncle && uncle->color == RED)  //情况1：z的叔叔y是红色的
            {
                uncle->color = BLACK;
                parent->color = BLACK;
                gparent->color = RED;
                node = gparent;           //同上。
            }
            else                               //情况2：z的叔叔y是黑色的，
            {
                if (parent->left == node)  //且z为左孩子
                {
                    root = rb_rotate_right(parent, root);  //以结点parent、root右旋
                    tmp = parent;
                    parent = node;
                    node = tmp;       //parent与node 互换角色
                }
                  //经过情况2的变化，成为了情况3.
                parent->color = BLACK;
                gparent->color = RED;
                root = rb_rotate_left(gparent, root);   //以结点gparent和root左旋
            }
        }
    }

    root->color = BLACK; //根结点，不论怎样，都得置为黑色。
    return root;      //返回根结点。
}


//六、红黑树的删除
//------------------------------------------------------------
//红黑树的删除结点
void rbp_erase(rb_map_t *rbp, rb_key_t key)
{
    
    rb_node_t *child, *parent, *old, *left, *node;
    color_t color;

    if (!(node = rbp_search_auxiliary(key, rbp->root, NULL)))  //调用rbp_search_auxiliary查找要删除的结点
    {
        //printf("key %d is not exist!\n");
        return ;
    }

    old = node;

    if (node->left && node->right)
    {
        node = node->right;
        while ((left = node->left) != NULL)
        {
            node = left;
        }
        child = node->right;
        parent = node->parent;
        color = node->color;

        if (child)
        {
            child->parent = parent;
        }
        if (parent)
        {
            if (parent->left == node)
            {
                parent->left = child;
            }
            else
            {
                parent->right = child;
            }
        }
        else
        {
            rbp->root = child;
        }

        if (node->parent == old)
        {
            parent = node;
        }

        node->parent = old->parent;
        node->color = old->color;
        node->right = old->right;
        node->left = old->left;

        if (old->parent)
        {
            if (old->parent->left == old)
            {
                old->parent->left = node;
            }
            else
            {
                old->parent->right = node;
            }
        }
        else
        {
            rbp->root = node;
        }

        old->left->parent = node;
        if (old->right)
        {
            old->right->parent = node;
        }
    }
    else
    {
        if (!node->left)
        {
            child = node->right;
        }
        else if (!node->right)
        {
            child = node->left;
        }
        parent = node->parent;
        color = node->color;

        if (child)
        {
            child->parent = parent;
        }
        if (parent)
        {
            if (parent->left == node)
            {
                parent->left = child;
            }
            else
            {
                parent->right = child;
            }
        }
        else
        {
            rbp->root = child;
        }
    }

    rbp->rb_destory(old);

    if (color == BLACK)
    {
        rbp->root = rbp_erase_rebalance(child, parent, rbp->root); //调用rbp_erase_rebalance来恢复红黑树性质
    }
}


//七、红黑树的4种删除情况
//----------------------------------------------------------------
//红黑树修复删除的4种情况
//为了表示下述注释的方便，也为了让下述代码与我的倆篇文章相对应，
//x表示要删除的结点，*other、w表示兄弟结点，
//----------------------------------------------------------------
static rb_node_t* rbp_erase_rebalance(rb_node_t *node, rb_node_t *parent, rb_node_t *root)
{
    rb_node_t *other, *o_left, *o_right;   //x的兄弟*other，兄弟左孩子*o_left,*o_right

    while ((!node || node->color == BLACK) && node != root)
    {
        if (parent->left == node)
        {
            other = parent->right;
            if (other->color == RED)   //情况1：x的兄弟w是红色的
            {
                other->color = BLACK;
                parent->color = RED;   //上俩行，改变颜色，w->黑、p[x]->红。
                root = rb_rotate_left(parent, root);  //再对p[x]做一次左旋
                other = parent->right;  //x的新兄弟new w 是旋转之前w的某个孩子。其实就是左旋后的效果。
            }
            if ((!other->left || other->left->color == BLACK) &&
                (!other->right || other->right->color == BLACK))
                          //情况2：x的兄弟w是黑色，且w的俩个孩子也都是黑色的

            {                         //由于w和w的俩个孩子都是黑色的，则在x和w上得去掉一黑色，
                other->color = RED;   //于是，兄弟w变为红色。
                node = parent;    //p[x]为新结点x
                parent = node->parent;  //x<-p[x]
            }
            else                       //情况3：x的兄弟w是黑色的，
            {                          //且，w的左孩子是红色，右孩子为黑色。
                if (!other->right || other->right->color == BLACK)
                {
                    if ((o_left = other->left))   //w和其左孩子left[w]，颜色交换。
                    {
                        o_left->color = BLACK;    //w的左孩子变为由黑->红色
                    }
                    other->color = RED;           //w由黑->红
                    root = rb_rotate_right(other, root);  //再对w进行右旋，从而红黑性质恢复。
                    other = parent->right;        //变化后的，父结点的右孩子，作为新的兄弟结点w。
                }
                            //情况4：x的兄弟w是黑色的

                other->color = parent->color;  //把兄弟节点染成当前节点父节点的颜色。
                parent->color = BLACK;  //把当前节点父节点染成黑色
                if (other->right)      //且w的右孩子是红
                {
                    other->right->color = BLACK;  //兄弟节点w右孩子染成黑色
                }
                root = rb_rotate_left(parent, root);  //并再做一次左旋
                node = root;   //并把x置为根。
                break;
            }
        }
  //下述情况与上述情况，原理一致。分析略。
        else
        {
            other = parent->left;
            if (other->color == RED)
            {
                other->color = BLACK;
                parent->color = RED;
                root = rb_rotate_right(parent, root);
                other = parent->left;
            }
            if ((!other->left || other->left->color == BLACK) &&(!other->right || other->right->color == BLACK))
            {
                other->color = RED;
                node = parent;
                parent = node->parent;
            }
            else
            {
                if (!other->left || other->left->color == BLACK)
                {
                    if ((o_right = other->right))
                    {
                        o_right->color = BLACK;
                    }
                    other->color = RED;
                    root = rb_rotate_left(other, root);
                    other = parent->left;
                }
                other->color = parent->color;
                parent->color = BLACK;
                if (other->left)
                {
                    other->left->color = BLACK;
                }
                root = rb_rotate_right(parent, root);
                node = root;
                break;
            }
        }
    }

    if (node)
    {
        node->color = BLACK;  //最后将node[上述步骤置为了根结点]，改为黑色。
    }
    return root;  //返回root
}


//八、测试用例
//主函数
/*
int main()
{
    u_int32_t i, count = 100;
    rb_key_t key;
    rb_map_t *rbp;
    rb_node_t *node = NULL;
    srand(time(NULL));
    rbp = alloc_rbp(malloc, free);
    if(!rbp){
        fprintf(stderr,"alloc rbp fail\n");
    }
    rbp_set(rbp, 0, 1);
    rbp_set(rbp, 2, 2);
    rbp_set(rbp, 3, 4);
    rbp_set(rbp, 4, 1);
    rbp_set(rbp, 2, 7);
    rbp_set(rbp, 4, 1);
    for(i = 0; i < 10; i++){
            fprintf(stderr, "key %u\n:", i);
            if(node = rbp_search(rbp, i)){
                fprintf(stderr, "%u\n", node->data);
            }else{
                fprintf(stderr, "not exists\n");
            }
    }
    rbp_erase(rbp, 1);
    rbp_erase(rbp, 2);
    rbp_erase(rbp, 3);
    rbp_erase(rbp, 9);
    rbp_erase(rbp, 22);
    rbp_set(rbp, 1, 7);
    for(i = 0; i < 10; i++){
            fprintf(stderr, "key %u\n:", i);
            if(node = rbp_search(rbp, i)){
                fprintf(stderr, "%u\n", node->data);
            }else{
                fprintf(stderr, "not exists\n");
            }
    }
    return 0;
}
*/
