class AbstractIndex
{
private:
public:
    AbstractIndex();
    ~AbstractIndex();
    virtual void adapt_index() = 0;
    virtual void range_query() = 0;
};

AbstractIndex::AbstractIndex()
{
}

AbstractIndex::~AbstractIndex()
{
}
