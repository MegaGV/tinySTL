#ifndef __TINYSTL_RBTREE_IMPL_H
#define __TINYSTL_RBTREE_IMPL_H

namespace TinySTL
{

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::rbtree_node_ptr rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::getNode()
	{
		return rb_tree_node_allocator::allocate();
	}


	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::rbtree_node_ptr rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::createNode(const Value& val)
	{
		rbtree_node_ptr res = getNode();
		construct(&res->value, val);
		//并未赋值三个指针
		return res;
	}

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::putNode(rbtree_node_ptr p)
	{
		rb_tree_node_allocator::deallocate(p);
	}

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::deleteNode(rbtree_node_ptr p)
	{
		destroy(&p->value);
		putNode(p);
	}

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::init()
	{
		header = createNode();
		leftmost() = header;
		rightmost() = header;
		color(header) = _red;
		root() = 0;
	}

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::rb_tree(const Compare& comp) :node_count(0), key_compare(comp)
	{
		init();
	}

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	pair<typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator, bool> rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_unique(const Value& val)
	{
		rbtree_node_ptr p = header;
		rbtree_node_ptr x = root();
		bool res = true;;
		while (x != 0)
		{
			p = x;
			res = key_compare(KeyOfValue()(val), key(x));
			if (res)
			{
				x = left(x);
			}
			else
			{
				x = right(x);
			}
		}
		//尚未有一个节点，此时根本没有进入while循环
		if (p == header)
		{
			return pair<iterator, bool>(__insert(x, p, val), true);
		}

		iterator j = iterator(p);
		if (res)
		{
			if (j == begin())
			{
				return pair<iterator, bool>(__insert(x, p, val), true);
			}
			else
			{
				--j;
			}
		}

		if (key_compare(key(j.p), KeyOfValue()(val)))
		{
			return pair<iterator, bool>(__insert(x, p, val), true);
		}

		return pair<iterator, bool>(j, false);


	}

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::__insert(rbtree_node_ptr x, rbtree_node_ptr y, const Value& val)
	{
		rbtree_node_ptr z;
		if (y == header || key_compare(KeyOfValue()(val), key(y)))
		{
			z = createNode(val);
			left(y) = z;
			if (y == header)
			{
				root() = z;
				rightmost() = z;
			}
			else if (y == leftmost())
			{
				leftmost() = z;
			}

		}
		else
		{
			z = createNode(val);
			right(y) = z;
			if (rightmost() == y)
			{
				rightmost() = z;
			}
		}

		parent(z) = y;
		left(z) = 0;
		right(z) = 0;

		rb_tree_rebalance(z, parent(header));
		++node_count;
		return iterator(z);


	}

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::rb_tree_rebalance(rbtree_node_ptr z, rbtree_node_ptr& root)
	{
		z->color = _red;
		while (z != root && z->parent->color == _red)
		{
			if (z->parent == ((rbtree_node_ptr)(z->parent->parent->left)))
			{
				rbtree_node_ptr s = (rbtree_node_ptr)z->parent->parent->right;
				if (s != 0 && s->color == _red)
				{
					s->color = _black;
					z->parent->color = _black;
					z->parent->parent->color = _red;
					z = (rbtree_node_ptr)z->parent->parent;

				}
				else
				{
					if (z == (rbtree_node_ptr)z->parent->right)
					{
						z = (rbtree_node_ptr)z->parent;
						rb_tree_rotate_left(z, root);
					}
					z->parent->parent->color = _red;
					z->parent->color = _black;
					rb_tree_rotate_right((rbtree_node_ptr)z->parent->parent, root);
				}

			}
			else
			{
				rbtree_node_ptr s = (rbtree_node_ptr)z->parent->parent->left;
				if (s != 0 && s->color == _red)
				{
					s->color = _black;
					z->parent->color = _black;
					z->parent->parent->color = _red;
					z = (rbtree_node_ptr)z->parent->parent;
				}
				else
				{
					if (z == (rbtree_node_ptr)z->parent->left)
					{
						z = (rbtree_node_ptr)z->parent;
						rb_tree_rotate_right(z, root);
					}
					z->parent->parent->color = _red;
					z->parent->color = _black;
					rb_tree_rotate_left((rbtree_node_ptr)z->parent->parent, root);

				}

			}


		}
		root->color = _black;
	}

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::rb_tree_rotate_left(rbtree_node_ptr x, rbtree_node_ptr& root)
	{
		rbtree_node_ptr y = right(x);
		right(x) = left(y);
		if (y->left != 0)
			parent(left(y)) = x;

		parent(y) = parent(x);
		if (x == root)
			root = y;
		else if (left(parent(x)) == x)
		{
			left(parent(x)) = y;
		}
		else
		{
			right(parent(x)) = y;
		}

		parent(x) = y;
		left(y) = x;

	}


	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::rb_tree_rotate_right(rbtree_node_ptr x, rbtree_node_ptr& root)
	{
		rbtree_node_ptr y = left(x);
		left(x) = right(y);
		if (y->right != 0)
			parent(right(y)) = x;

		parent(y) = parent(x);
		if (x == root)
			y = root;
		else if (left(parent(x)) == x)
		{
			left(parent(x)) = y;
		}
		else
		{
			right(parent(x)) = y;
		}

		parent(x) = y;
		right(y) = x;
	}
	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::recurErase(rbtree_node_ptr x)
	{
		if (x != 0)
		{
			recurErase(left(x));
			recurErase(right(x));
			deleteNode(x);
		}
	}

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::clear()
	{
		recurErase(root());
		left(header) = header;
		right(header) = header;
		parent(header) = 0;


	}

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::~rb_tree()
	{
		clear();
		deleteNode(header);
	}

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::rbtree_node_ptr
	rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::findRBTree(const value_type& val, bool& isFind)
	{
		rbtree_node_ptr res = parent(header);
		isFind = false;
		while (res != 0)
		{
			if (key_compare(KeyOfValue()(val), key(res)))
			{
				res = left(res);
			}
			else
			{
				if (key_compare(key(res), KeyOfValue()(val)))
				{
					res = right(res);
				}
				else
				{
					isFind = true;
					break;
				}
			}
		}
		return res;
	}

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::find(const value_type& val)
	{
		bool isFind = false;
		rbtree_node_ptr res = findRBTree(val, isFind);
		if (isFind)
		{
			return iterator(res);
		}
		else
		{
			return end();
		}
	}

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(iterator position)
	{
		rbtree_node_ptr to_be_delete = rb_tree_rebalance_for_erase(position.p, root(), leftmost(), rightmost());
		deleteNode(to_be_delete);
		--node_count;
	}


	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::rbtree_node_ptr
		rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::rb_tree_rebalance_for_erase(rbtree_node_ptr z, rbtree_node_ptr& root, rbtree_node_ptr& leftmost, rbtree_node_ptr& rightmost)
	{
		rbtree_node_ptr y = z;		//实际删除的节点
		rbtree_node_ptr x = 0;     //替代z(y)的节点
		rbtree_node_ptr x_parent;	//删除之后 x 的父节点

		if (left(y) == 0)                 //这里帮找实际需要删除的y节点  以及 y的子节点 x(上位y)
		{
			x = right(y);
		}
		else if (right(y) == 0)
		{
			x = left(y);
		}
		else
		{
			y = right(y);
			while (left(y) != 0)
				y = left(y);
			x = right(y);
		}

		if (y == z)
		{
			x_parent = parent(y);
			if (z == root)
			{
				root = x;
			}
			else if (z == left(parent(z)))
			{
				left(parent(z)) = x;
			}
			else
			{
				right(parent(z)) = x;
			}

			if (x != 0) parent(x) = parent(z);

			if (z == leftmost)
			{
				if (right(z) == 0)
					leftmost = parent(z);
				else
					leftmost = (rbtree_node_ptr)(RBTreeMinimum(x));
			}

			if (z == rightmost)
			{
				if (z->left == 0)
					rightmost = parent(z);
				else
					rightmost = (rbtree_node_ptr)(RBTreeMaximum(x));
			}
		}
		else
		{
			left(y) = left(z);
			parent(left(z)) = y;
			if (right(z) != y)
			{
				x_parent = parent(y);
				if (x != 0) parent(x) = parent(y);
				left(parent(y)) = x;

				right(y) = right(z);
				parent(right(z)) = y;
			}
			else
			{
				x_parent = y;
			}

			if (z == root)
			{
				root = y;
			}
			else if (z == left(parent(z)))
			{
				left(parent(z)) = y;
			}
			else
			{
				right(parent(z)) = y;
			}
			parent(y) = parent(z);

			swap(y->color, z->color);
			y = z;
		}

		// y： 待删节点   x： 替换y的节点(鸠占鹊巢),此时x也成为标记节点   x_parent: x的父节点
		if (y->color != _red)
		{
			while (x != root && (x == 0 || x->color == _black))  //为双黑色节点
			{
				if (x == left(x_parent))
				{
					rbtree_node_ptr w = right(x_parent);
					if (w->color == _red)
					{
						w->color = _black;
						x_parent->color = _red;
						rb_tree_rotate_left(x_parent, root);
						w = right(x_parent);
					}
					if ((left(w) == 0 || color(left(w)) == _black) && (right(w) == 0 || color(right(w)) == _black))
					{
						w->color = _red;
						x = x_parent;
						x_parent = parent(x_parent);
					}
					else
					{
						if (right(w) == 0 || color(right(w)) == _black)
						{
							if (left(w) != 0)
								color(left(w)) = _black;
							color(right(w)) = _red;
							rb_tree_rotate_right(w, root);
							w = right(x_parent);
						}
						w->color = x_parent->color;
						x_parent->color = _black;
						if (w->right != 0)
							color(right(w)) = _black;
						rb_tree_rotate_left(x_parent, root);
						break;
					}

				}
				else
				{
					rbtree_node_ptr w = left(x_parent);
					if (w->color == _red)
					{
						w->color = _black;
						x_parent->color = _red;
						rb_tree_rotate_right(x_parent, root);
						w = left(x_parent);
					}

					if ((left(w) == 0 || color(left(w)) == _black) && (right(w) == 0 || color(right(w)) == _black))
					{
						w->color = _red;
						x = x_parent;
						x_parent = parent(x_parent);
					}
					else
					{
						if (left(w) == 0 ||color(left(w)) == _black)
						{
							if (right(w) != 0)
								 color(right(w)) = _black;
							w->color = _red;
							rb_tree_rotate_left(w, root);
							w = left(x_parent);
						}
						w->color = x_parent->color;
						x_parent->color = _black;
						if (w->left != 0)
							color(left(w)) = _black;
						rb_tree_rotate_right(x_parent, root);
						break;
					}


				}
			}
			if (x != 0) x->color = _black;
		}

		return y;


	}
}

#endif