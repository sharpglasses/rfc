#include "rb_map.h"

/**
 * @brief rb_map copy from july
 * @file rb_map.c
 * @author july
 * @date 03/11/2014
 * @note change from july's blog from csdn 
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


//һ�������������
/*-----------------------------------------------------------
|   node           right
|   / \    ==>     / \
|   a  right     node  y
|       / \       / \
|       b  y     a   b    //����
-----------------------------------------------------------*/
static rb_node_t* rb_rotate_left(rb_node_t* node, rb_node_t* root)
{
    rb_node_t* right = node->right;    //ָ��ָ��ָ�� right<--node->right

    if ((node->right = right->left))
    {
        right->left->parent = node;  //�ñ������ע��ͼ��node��Ϊb�ĸ�ĸ
    }
    right->left = node;   //node��Ϊright������

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
    node->parent = right;  //right��Ϊnode�ĸ�ĸ

    return root;
}


//��������
/*-----------------------------------------------------------
|       node            left
|       / \             / \
|    left  y   ==>    a    node
|   / \                    / \
|  a   b                  b   y  //������������࣬�����Թ�
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


//������������ҽ��
//----------------------------------------------------
//rbp_search_auxiliary������
//rb_node_t* rbp_search�������ҵ��Ľ��
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

//��������rbp_search_auxiliary���ҽ��
rb_node_t* rbp_search(rb_map_t *rbp, rb_key_t key)
{
    return rbp_search_auxiliary(key, rbp->root, NULL);
}


//�ġ�������Ĳ���
//---------------------------------------------------------
//������Ĳ�����
s_int32_t rbp_set(rb_map_t *rbp, rb_key_t key, rb_data_t data)
{
    rb_node_t *parent = NULL, *node;

    parent = NULL;
    if ((node = rbp_search_auxiliary(key, rbp->root, &parent)))  //����rbp_search_auxiliary�ҵ�������ĵط�

    {
        node->data = data;
        return 0;
    }

    node = rb_new_node(rbp, key, data);  //������
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
    rbp->root = rbp_set_rebalance(node, rbp->root);   //������󣬵���rbp_set_rebalance�޸������������
    return 0;
}


//�塢�������3�ֲ������
//�������������ص������Ժ���������3������������е��޸�������
//--------------------------------------------------------------
//������޸������3�����
//Ϊ���������ע���б�ʾ���㣬ҲΪ���������������ҵĂzƪ�������Ӧ��
//��z��ʾ��ǰ��㣬p[z]��ʾ��ĸ��p[p[z]]��ʾ�游��y��ʾ���塣
//--------------------------------------------------------------
static rb_node_t * rbp_set_rebalance(rb_node_t *node, rb_node_t *root)
{
    rb_node_t *parent, *gparent, *uncle, *tmp;  //��ĸp[z]���游p[p[z]]������y����ʱ���*tmp

    while ((parent = node->parent) && parent->color == RED)
    {     //parent Ϊnode�ĸ�ĸ���ҵ���ĸ����ɫΪ��ʱ
        gparent = parent->parent;   //gparentΪ�游

        if (parent == gparent->left)  //���游�����Ӽ�Ϊ��ĸʱ��
                                 //��ʵ����������䣬�޷Ǿ�����˳���ӡ���ĸ���游�Ĺ�ϵ��:D��
        {
            uncle = gparent->right;  //��������ĸ������y���Ǹ�ĸ���Һ��ӡ�

            if (uncle && uncle->color == RED) //���1��z������y�Ǻ�ɫ��
            {
                uncle->color = BLACK;   //��������y��Ϊ��ɫ
                parent->color = BLACK;  //z�ĸ�ĸp[z]Ҳ��Ϊ��ɫ�����z��p[z]���Ǻ�ɫ�����⡣
                gparent->color = RED;
                node = gparent;     //���游�����������z��ָ��z�������㣬����Ϊ��ɫ��
    //�������1�У�ֻ������z��Ϊ��ĸ���Һ��ӵ������
            }
            else                     //���2��z������y�Ǻ�ɫ�ģ�
            {
                if (parent->right == node)  //��zΪ�Һ���
                {
                    root = rb_rotate_left(parent, root); //����[���z���븸ĸ���]
                    tmp = parent;
                    parent = node;
                    node = tmp;     //parent��node ������ɫ
                }
                             //���3��z������y�Ǻ�ɫ�ģ���ʱz��Ϊ�����ӡ�
                                    //ע�⣬1�����3�����������2�仯�����ġ�
                                    //......2��z���������Ǻ�ɫ�ģ�����������1�ˡ�
                parent->color = BLACK;   //z�ĸ�ĸp[z]��Ϊ��ɫ
                gparent->color = RED;    //ԭ�游�����Ϊ��ɫ
                root = rb_rotate_right(gparent, root); //����[���z�����游���]
            }
        }

        else
        {
   //�ⲿ�����ر�Ϊ���1�У�z��Ϊ�����������д�ġ�
            uncle = gparent->left;  //�游��������Ϊ�����㡣[ԭ�������ϲ���һ����]
            if (uncle && uncle->color == RED)  //���1��z������y�Ǻ�ɫ��
            {
                uncle->color = BLACK;
                parent->color = BLACK;
                gparent->color = RED;
                node = gparent;           //ͬ�ϡ�
            }
            else                               //���2��z������y�Ǻ�ɫ�ģ�
            {
                if (parent->left == node)  //��zΪ����
                {
                    root = rb_rotate_right(parent, root);  //�Խ��parent��root����
                    tmp = parent;
                    parent = node;
                    node = tmp;       //parent��node ������ɫ
                }
                  //�������2�ı仯����Ϊ�����3.
                parent->color = BLACK;
                gparent->color = RED;
                root = rb_rotate_left(gparent, root);   //�Խ��gparent��root����
            }
        }
    }

    root->color = BLACK; //����㣬����������������Ϊ��ɫ��
    return root;      //���ظ���㡣
}


//�����������ɾ��
//------------------------------------------------------------
//�������ɾ�����
void rbp_erase(rb_map_t *rbp, rb_key_t key)
{
    
    rb_node_t *child, *parent, *old, *left, *node;
    color_t color;

    if (!(node = rbp_search_auxiliary(key, rbp->root, NULL)))  //����rbp_search_auxiliary����Ҫɾ���Ľ��
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
        rbp->root = rbp_erase_rebalance(child, parent, rbp->root); //����rbp_erase_rebalance���ָ����������
    }
}


//�ߡ��������4��ɾ�����
//----------------------------------------------------------------
//������޸�ɾ����4�����
//Ϊ�˱�ʾ����ע�͵ķ��㣬ҲΪ���������������ҵĂzƪ�������Ӧ��
//x��ʾҪɾ���Ľ�㣬*other��w��ʾ�ֵܽ�㣬
//----------------------------------------------------------------
static rb_node_t* rbp_erase_rebalance(rb_node_t *node, rb_node_t *parent, rb_node_t *root)
{
    rb_node_t *other, *o_left, *o_right;   //x���ֵ�*other���ֵ�����*o_left,*o_right

    while ((!node || node->color == BLACK) && node != root)
    {
        if (parent->left == node)
        {
            other = parent->right;
            if (other->color == RED)   //���1��x���ֵ�w�Ǻ�ɫ��
            {
                other->color = BLACK;
                parent->color = RED;   //�����У��ı���ɫ��w->�ڡ�p[x]->�졣
                root = rb_rotate_left(parent, root);  //�ٶ�p[x]��һ������
                other = parent->right;  //x�����ֵ�new w ����ת֮ǰw��ĳ�����ӡ���ʵ�����������Ч����
            }
            if ((!other->left || other->left->color == BLACK) &&
                (!other->right || other->right->color == BLACK))
                          //���2��x���ֵ�w�Ǻ�ɫ����w����������Ҳ���Ǻ�ɫ��

            {                         //����w��w���������Ӷ��Ǻ�ɫ�ģ�����x��w�ϵ�ȥ��һ��ɫ��
                other->color = RED;   //���ǣ��ֵ�w��Ϊ��ɫ��
                node = parent;    //p[x]Ϊ�½��x
                parent = node->parent;  //x<-p[x]
            }
            else                       //���3��x���ֵ�w�Ǻ�ɫ�ģ�
            {                          //�ң�w�������Ǻ�ɫ���Һ���Ϊ��ɫ��
                if (!other->right || other->right->color == BLACK)
                {
                    if ((o_left = other->left))   //w��������left[w]����ɫ������
                    {
                        o_left->color = BLACK;    //w�����ӱ�Ϊ�ɺ�->��ɫ
                    }
                    other->color = RED;           //w�ɺ�->��
                    root = rb_rotate_right(other, root);  //�ٶ�w�����������Ӷ�������ʻָ���
                    other = parent->right;        //�仯��ģ��������Һ��ӣ���Ϊ�µ��ֵܽ��w��
                }
                            //���4��x���ֵ�w�Ǻ�ɫ��

                other->color = parent->color;  //���ֵܽڵ�Ⱦ�ɵ�ǰ�ڵ㸸�ڵ����ɫ��
                parent->color = BLACK;  //�ѵ�ǰ�ڵ㸸�ڵ�Ⱦ�ɺ�ɫ
                if (other->right)      //��w���Һ����Ǻ�
                {
                    other->right->color = BLACK;  //�ֵܽڵ�w�Һ���Ⱦ�ɺ�ɫ
                }
                root = rb_rotate_left(parent, root);  //������һ������
                node = root;   //����x��Ϊ����
                break;
            }
        }
  //������������������ԭ��һ�¡������ԡ�
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
        node->color = BLACK;  //���node[����������Ϊ�˸����]����Ϊ��ɫ��
    }
    return root;  //����root
}


//�ˡ���������
//������
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
