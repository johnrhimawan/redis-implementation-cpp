namespace redis::session {

class ClientSession {
  public:
    explicit ClientSession(int fd);
    ~ClientSession();

    int fd() const;
    bool onReadable();

  private:
    int _fd;
};

} // namespace
