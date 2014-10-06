#ifndef PTP_H
#define	PTP_H

class PTP {
public:
  PTP();
  PTP(const PTP& orig);
  virtual ~PTP();
private:
  void setProfessores();
  void setDisciplinas();
};

#endif	/* PTP_H */

