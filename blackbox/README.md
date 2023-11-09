    +---------------------+
    |     Audio src       |
    | [batch, input_samples]|
    +---------------------+
            |
            v
    +---------------------+
    |     Resistance      |
    | [batch, resistance_dim]|
    +---------------------+
            |
            v
    +---------------------+
    |     Embedding       |
    | [batch, d_model]    |
    +---------------------+
            |
            v
    +---------------------+
    |    Transformer      |
    +---------------------+
    | +-----------------+ |
    | |  Encoder Layer  | |
    | |  (Multi-head   | |
    | |   Attention)   | |
    | +-----------------+ |
    |         .           |
    |         .           |
    |         .           |
    | +-----------------+ |
    | |  Encoder Layer  | |
    | |  (Multi-head   | |
    | |   Attention)   | |
    | +-----------------+ |
    |         |           |
    | +-----------------+ |
    | |  Decoder Layer  | |
    | |  (Multi-head   | |
    | |   Attention)   | |
    | +-----------------+ |
    |         .           |
    |         .           |
    |         .           |
    | +-----------------+ |
    | |  Decoder Layer  | |
    | |  (Multi-head   | |
    | |   Attention)   | |
    | +-----------------+ |
    +---------------------+
            |
            v
    +---------------------+
    |        FC           |
    | [batch, input_samples]|
    +---------------------+
            |
            v
    +---------------------+
    |     Output Audio    |
    | [batch, input_samples]|
    +---------------------+