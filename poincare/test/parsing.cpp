#include <poincare/init.h>
#include <poincare/exception_checkpoint.h>
#include <ion.h>
#include <cmath>
#include <assert.h>
#include <poincare/src/parsing/parser.h>
#include "tree/helpers.h"
#include "helper.h"

using namespace Poincare;

void assert_tokenizes_as(const Token::Type * tokenTypes, const char * string) {
  Tokenizer tokenizer(string);
  while (true) {
    Token token = tokenizer.popToken();
    quiz_assert_print_if_failure(token.type() == *tokenTypes, string);
    if (token.type() == Token::EndOfStream) {
      return;
    }
    tokenTypes++;
  }
}

void assert_tokenizes_as_number(const char * string) {
  const Token::Type types[] = {Token::Number, Token::EndOfStream};
  assert_tokenizes_as(types, string);
}

void assert_tokenizes_as_undefined_token(const char * string) {
  Tokenizer tokenizer(string);
  while (true) {
    Token token = tokenizer.popToken();
    if (token.type() == Token::Undefined) {
      return;
    }
    quiz_assert_print_if_failure(token.type() != Token::EndOfStream, string);
  }
}

void assert_text_not_parsable(const char * text) {
  Parser p(text);
  Expression result = p.parse();
  quiz_assert_print_if_failure(p.getStatus() != Parser::Status::Success, text);
}

void assert_parsed_expression_is(const char * expression, Poincare::Expression r, bool addParentheses = false) {
  Expression e = parse_expression(expression, addParentheses);
  quiz_assert_print_if_failure(e.isIdenticalTo(r), expression);
}

void assert_parsed_expression_with_user_parentheses_is(const char * expression, Poincare::Expression r) { return assert_parsed_expression_is(expression, r, true); }

QUIZ_CASE(poincare_parsing_tokenize_numbers) {
  assert_tokenizes_as_number("1");
  assert_tokenizes_as_number("12");
  assert_tokenizes_as_number("123");
  assert_tokenizes_as_number("1.3");
  assert_tokenizes_as_number(".3");
  assert_tokenizes_as_number("1.3ᴇ3");
  assert_tokenizes_as_number("12.34ᴇ56");
  assert_tokenizes_as_number(".3ᴇ-32");
  assert_tokenizes_as_number("12.");
  assert_tokenizes_as_number(".999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999");
  assert_tokenizes_as_number("0.");
  assert_tokenizes_as_number("1.ᴇ-4");
  assert_tokenizes_as_number("1.ᴇ9999");

  assert_tokenizes_as_undefined_token("1ᴇ");
  assert_tokenizes_as_undefined_token("1..");
  assert_tokenizes_as_undefined_token("..");
  assert_tokenizes_as_undefined_token("1.ᴇᴇ");
  assert_tokenizes_as_undefined_token("1.ᴇ");
  assert_tokenizes_as_undefined_token("1ᴇ--4");
  assert_tokenizes_as_undefined_token("1.ᴇᴇ4");
  assert_tokenizes_as_undefined_token("1ᴇ2ᴇ4");
}

QUIZ_CASE(poincare_parsing_memory_exhaustion) {
  int initialPoolSize = pool_size();
  assert_parsed_expression_is("2+3",Addition::Builder(Rational::Builder(2), Rational::Builder(3)));
  assert_pool_size(initialPoolSize);

  int memoryFailureHasBeenHandled = false;
  {
    Poincare::ExceptionCheckpoint ecp;
    if (ExceptionRun(ecp)) {
      Addition a = Addition::Builder();
      while (true) {
        Expression e = Expression::Parse("1+2+3+4+5+6+7+8+9+10");
        a.addChildAtIndexInPlace(e, 0, a.numberOfChildren());
      }
    } else {
      Poincare::Tidy();
      memoryFailureHasBeenHandled = true;
    }
  }

  quiz_assert(memoryFailureHasBeenHandled);
  assert_pool_size(initialPoolSize);
  Expression e = Expression::Parse("1+1");
  /* Stupid check to make sure the global variable generated by Bison is not
   * ruining everything */
}

QUIZ_CASE(poincare_parsing_parse_numbers) {
  // Parse decimal
  assert_parsed_expression_is("0", Rational::Builder(0));
  assert_parsed_expression_is("0.1", Decimal::Builder(0.1));
  assert_parsed_expression_is("1.", Rational::Builder(1));
  assert_parsed_expression_is(".1", Decimal::Builder(0.1));
  assert_parsed_expression_is("0ᴇ2", Decimal::Builder(0.0));
  assert_parsed_expression_is("0.1ᴇ2", Decimal::Builder(10.0));
  assert_parsed_expression_is("1.ᴇ2", Decimal::Builder(100.0));
  assert_parsed_expression_is(".1ᴇ2", Decimal::Builder(10.0));
  assert_parsed_expression_is("0ᴇ-2", Decimal::Builder(0.0));
  assert_parsed_expression_is("0.1ᴇ-2", Decimal::Builder(0.001));
  assert_parsed_expression_is("1.ᴇ-2", Decimal::Builder(0.01));
  assert_parsed_expression_is(".1ᴇ-2", Decimal::Builder(0.001));
  // Decimal with rounding when digits are above 14
  assert_parsed_expression_is("0.0000012345678901234", Decimal::Builder(Integer("12345678901234"), -6));
  assert_parsed_expression_is("0.00000123456789012345", Decimal::Builder(Integer("12345678901235"), -6));
  assert_parsed_expression_is("0.00000123456789012341", Decimal::Builder(Integer("12345678901234"), -6));
  assert_parsed_expression_is("1234567890123.4", Decimal::Builder(Integer("12345678901234"), 12));
  assert_parsed_expression_is("123456789012345.2", Decimal::Builder(Integer("12345678901235"), 14));
  assert_parsed_expression_is("123456789012341.2", Decimal::Builder(Integer("12345678901234"), 14));
  assert_parsed_expression_is("12.34567", Decimal::Builder(Integer("1234567"), 1));
  assert_parsed_expression_is(".999999999999990", Decimal::Builder(Integer("99999999999999"), -1));
  assert_parsed_expression_is("9.99999999999994", Decimal::Builder(Integer("99999999999999"), 0));
  assert_parsed_expression_is("99.9999999999995", Decimal::Builder(Integer("100000000000000"), 2));
  assert_parsed_expression_is("999.999999999999", Decimal::Builder(Integer("100000000000000"), 3));
  assert_parsed_expression_is("9999.99199999999", Decimal::Builder(Integer("99999920000000"), 3));
  assert_parsed_expression_is("99299.9999999999", Decimal::Builder(Integer("99300000000000"), 4));

  // Parse integer
  assert_parsed_expression_is("123456789012345678765434567", Rational::Builder("123456789012345678765434567"));
  assert_parsed_expression_is(MaxIntegerString(), Rational::Builder(MaxIntegerString()));

  // Integer parsed in Decimal because they overflow Integer
  assert_parsed_expression_is(OverflowedIntegerString(), Decimal::Builder(Integer("17976931348623"), 308));
  assert_parsed_expression_is("179769313486235590772930519078902473361797697894230657273430081157732675805500963132708477322407536021120113879871393357658789768814416622492847430639474124377767893424865485276302219601246094119453082952085005768838150682342462881473913110540827237163350510684586298239947245938479716304835356329624224137216", Decimal::Builder(Integer("17976931348624"), 308));

  // Infinity
  assert_parsed_expression_is("23ᴇ1000", Infinity::Builder(false));
  assert_parsed_expression_is("2.3ᴇ1000", Decimal::Builder(Integer(23), 1000));

  // Zero
  assert_parsed_expression_is("0.23ᴇ-1000", Decimal::Builder(Integer(0), 0));
  assert_parsed_expression_is("0.23ᴇ-999", Decimal::Builder(Integer(23), -1000));
}

QUIZ_CASE(poincare_parsing_parse) {
  assert_parsed_expression_is("1", Rational::Builder(1));
  assert_parsed_expression_is("(1)", Parenthesis::Builder(Rational::Builder(1)));
  assert_parsed_expression_is("((1))", Parenthesis::Builder((Expression)Parenthesis::Builder(Rational::Builder(1))));
  assert_parsed_expression_is("1+2", Addition::Builder(Rational::Builder(1),Rational::Builder(2)));
  assert_parsed_expression_is("(1)+2", Addition::Builder(Parenthesis::Builder(Rational::Builder(1)),Rational::Builder(2)));
  assert_parsed_expression_is("(1+2)", Parenthesis::Builder(Addition::Builder(Rational::Builder(1),Rational::Builder(2))));
  Expression nAryChildren[] = {Rational::Builder(1),Rational::Builder(2),Rational::Builder(3)};
  assert_parsed_expression_is("1+2+3", Addition::Builder(nAryChildren, 3));
  nAryChildren[2] = Parenthesis::Builder(Addition::Builder(Rational::Builder(3),Rational::Builder(4)));
  assert_parsed_expression_is("1+2+(3+4)", Addition::Builder(nAryChildren, 3));
  assert_parsed_expression_is("1×2", Multiplication::Builder(Rational::Builder(1),Rational::Builder(2)));
  nAryChildren[2] = Rational::Builder(3);
  assert_parsed_expression_is("1×2×3", Multiplication::Builder(nAryChildren, 3));
  assert_parsed_expression_is("1+2×3", Addition::Builder(Rational::Builder(1), Multiplication::Builder(Rational::Builder(2), Rational::Builder(3))));
  assert_parsed_expression_is("1/2", Division::Builder(Rational::Builder(1),Rational::Builder(2)));
  assert_parsed_expression_is("(1/2)", Parenthesis::Builder(Division::Builder(Rational::Builder(1),Rational::Builder(2))));
  assert_parsed_expression_is("1/2/3", Division::Builder(Division::Builder(Rational::Builder(1),Rational::Builder(2)),Rational::Builder(3)));
  assert_parsed_expression_is("1/2×3", Multiplication::Builder(Division::Builder(Rational::Builder(1),Rational::Builder(2)),Rational::Builder(3)));
  assert_parsed_expression_is("(1/2×3)", Parenthesis::Builder(Multiplication::Builder(Division::Builder(Rational::Builder(1),Rational::Builder(2)),Rational::Builder(3))));
  assert_parsed_expression_is("1×2/3", Multiplication::Builder(Rational::Builder(1),Division::Builder(Rational::Builder(2),Rational::Builder(3))));
  assert_parsed_expression_is("(1×2/3)", Parenthesis::Builder(Multiplication::Builder(Rational::Builder(1),Division::Builder(Rational::Builder(2),Rational::Builder(3)))));
  assert_parsed_expression_is("(1/2/3)", Parenthesis::Builder(Division::Builder(Division::Builder(Rational::Builder(1),Rational::Builder(2)),Rational::Builder(3))));
  assert_parsed_expression_is("1^2", Power::Builder(Rational::Builder(1),Rational::Builder(2)));
  assert_parsed_expression_is("1^2^3", Power::Builder(Rational::Builder(1),Power::Builder(Rational::Builder(2),Rational::Builder(3))));
  assert_parsed_expression_is("1=2", Equal::Builder(Rational::Builder(1),Rational::Builder(2)));
  assert_text_not_parsable("=5");
  assert_text_not_parsable("1=2=3");
  assert_parsed_expression_is("-1", Opposite::Builder(Rational::Builder(1)));
  assert_parsed_expression_is("(-1)", Parenthesis::Builder(Opposite::Builder(Rational::Builder(1))));
  assert_parsed_expression_is("1-2", Subtraction::Builder(Rational::Builder(1),Rational::Builder(2)));
  assert_parsed_expression_is("-1-2", Subtraction::Builder(Opposite::Builder(Rational::Builder(1)),Rational::Builder(2)));
  assert_parsed_expression_is("1-2-3", Subtraction::Builder(Subtraction::Builder(Rational::Builder(1),Rational::Builder(2)),Rational::Builder(3)));
  assert_parsed_expression_is("(1-2)", Parenthesis::Builder(Subtraction::Builder(Rational::Builder(1),Rational::Builder(2))));
  assert_parsed_expression_is("1+-2", Addition::Builder(Rational::Builder(1),Opposite::Builder(Rational::Builder(2))));
  assert_parsed_expression_is("--1", Opposite::Builder((Expression)Opposite::Builder(Rational::Builder(1))));
  assert_parsed_expression_is("(1+2)-3", Subtraction::Builder(Parenthesis::Builder(Addition::Builder(Rational::Builder(1),Rational::Builder(2))),Rational::Builder(3)));
  assert_parsed_expression_is("(2×-3)", Parenthesis::Builder(Multiplication::Builder(Rational::Builder(2),Opposite::Builder(Rational::Builder(3)))));
  assert_parsed_expression_is("1^(2)-3", Subtraction::Builder(Power::Builder(Rational::Builder(1),Parenthesis::Builder(Rational::Builder(2))),Rational::Builder(3)));
  assert_parsed_expression_is("1^2-3", Subtraction::Builder(Power::Builder(Rational::Builder(1),Rational::Builder(2)),Rational::Builder(3)));
  assert_parsed_expression_is("2^-3", Power::Builder(Rational::Builder(2),Opposite::Builder(Rational::Builder(3))));
  assert_parsed_expression_is("2--2+-1", Addition::Builder(Subtraction::Builder(Rational::Builder(2),Opposite::Builder(Rational::Builder(2))),Opposite::Builder(Rational::Builder(1))));
  assert_parsed_expression_is("2--2×-1", Subtraction::Builder(Rational::Builder(2),Opposite::Builder(Multiplication::Builder(Rational::Builder(2),Opposite::Builder(Rational::Builder(1))))));
  assert_parsed_expression_is("-1^2", Opposite::Builder(Power::Builder(Rational::Builder(1),Rational::Builder(2))));
  assert_parsed_expression_is("2ℯ^(3)", Multiplication::Builder(Rational::Builder(2),Power::Builder(Constant::Builder(UCodePointScriptSmallE),Parenthesis::Builder(Rational::Builder(3)))));
  assert_parsed_expression_is("2/-3/-4", Division::Builder(Division::Builder(Rational::Builder(2),Opposite::Builder(Rational::Builder(3))),Opposite::Builder(Rational::Builder(4))));
  assert_parsed_expression_is("1×2-3×4", Subtraction::Builder(Multiplication::Builder(Rational::Builder(1),Rational::Builder(2)),Multiplication::Builder(Rational::Builder(3),Rational::Builder(4))));
  assert_parsed_expression_is("-1×2", Opposite::Builder(Multiplication::Builder(Rational::Builder(1), Rational::Builder(2))));
  assert_parsed_expression_is("1!", Factorial::Builder(Rational::Builder(1)));
  assert_parsed_expression_is("1+2!", Addition::Builder(Rational::Builder(1),Factorial::Builder(Rational::Builder(2))));
  assert_parsed_expression_is("1!+2", Addition::Builder(Factorial::Builder(Rational::Builder(1)),Rational::Builder(2)));
  assert_parsed_expression_is("1!+2!", Addition::Builder(Factorial::Builder(Rational::Builder(1)),Factorial::Builder(Rational::Builder(2))));
  assert_parsed_expression_is("1×2!", Multiplication::Builder(Rational::Builder(1),Factorial::Builder(Rational::Builder(2))));
  assert_parsed_expression_is("1!×2", Multiplication::Builder(Factorial::Builder(Rational::Builder(1)),Rational::Builder(2)));
  assert_parsed_expression_is("1!×2!", Multiplication::Builder(Factorial::Builder(Rational::Builder(1)),Factorial::Builder(Rational::Builder(2))));
  assert_parsed_expression_is("1-2!", Subtraction::Builder(Rational::Builder(1),Factorial::Builder(Rational::Builder(2))));
  assert_parsed_expression_is("1!-2", Subtraction::Builder(Factorial::Builder(Rational::Builder(1)),Rational::Builder(2)));
  assert_parsed_expression_is("1!-2!", Subtraction::Builder(Factorial::Builder(Rational::Builder(1)),Factorial::Builder(Rational::Builder(2))));
  assert_parsed_expression_is("1/2!", Division::Builder(Rational::Builder(1),Factorial::Builder(Rational::Builder(2))));
  assert_parsed_expression_is("1!/2", Division::Builder(Factorial::Builder(Rational::Builder(1)),Rational::Builder(2)));
  assert_parsed_expression_is("1!/2!", Division::Builder(Factorial::Builder(Rational::Builder(1)),Factorial::Builder(Rational::Builder(2))));
  assert_parsed_expression_is("1^2!", Power::Builder(Rational::Builder(1),Factorial::Builder(Rational::Builder(2))));
  assert_parsed_expression_is("1!^2", Power::Builder(Factorial::Builder(Rational::Builder(1)),Rational::Builder(2)));
  assert_parsed_expression_is("1!^2!", Power::Builder(Factorial::Builder(Rational::Builder(1)),Factorial::Builder(Rational::Builder(2))));
  assert_parsed_expression_is("(1)!", Factorial::Builder(Parenthesis::Builder(Rational::Builder(1))));
  assert_text_not_parsable("1+");
  assert_text_not_parsable(")");
  assert_text_not_parsable(")(");
  assert_text_not_parsable("()");
  assert_text_not_parsable("(1");
  assert_text_not_parsable("1)");
  assert_text_not_parsable("1++2");
  assert_text_not_parsable("1//2");
  assert_text_not_parsable("×1");
  assert_text_not_parsable("1^^2");
  assert_text_not_parsable("^1");
  assert_text_not_parsable("t0000000");
  assert_text_not_parsable("[[t0000000[");
  assert_text_not_parsable("0→x=0");
  assert_text_not_parsable("0=0→x");
  assert_text_not_parsable("1ᴇ2ᴇ3");
}

Matrix BuildMatrix(int rows, int columns, Expression entries[]) {
  Matrix m = Matrix::Builder();
  int position = 0;
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      m.addChildAtIndexInPlace(entries[position], position, position);
      position++;
    }
  }
  m.setDimensions(rows, columns);
  return m;
}

QUIZ_CASE(poincare_parsing_matrices) {
  Expression m1[] = {Rational::Builder(1)};
  assert_parsed_expression_is("[[1]]", BuildMatrix(1,1,m1));
  Expression m2[] = {Rational::Builder(1),Rational::Builder(2),Rational::Builder(3)};
  assert_parsed_expression_is("[[1,2,3]]", BuildMatrix(1,3,m2));
  Expression m3[] = {Rational::Builder(1),Rational::Builder(2),Rational::Builder(3),Rational::Builder(4),Rational::Builder(5),Rational::Builder(6)};
  assert_parsed_expression_is("[[1,2,3][4,5,6]]", BuildMatrix(2,3,m3));
  Expression m4[] = {Rational::Builder(1), BuildMatrix(1,1,m1)};
  assert_parsed_expression_is("[[1,[[1]]]]", BuildMatrix(1,2,m4));
  assert_text_not_parsable("[");
  assert_text_not_parsable("]");
  assert_text_not_parsable("[[");
  assert_text_not_parsable("][");
  assert_text_not_parsable("[]");
  assert_text_not_parsable("[1]");
  assert_text_not_parsable("[[1,2],[3]]");
  assert_text_not_parsable("[[]");
  assert_text_not_parsable("[[1]");
  assert_text_not_parsable("[1]]");
  assert_text_not_parsable("[[,]]");
  assert_text_not_parsable("[[1,]]");
  assert_text_not_parsable(",");
  assert_text_not_parsable("[,]");
}

QUIZ_CASE(poincare_parsing_symbols_and_functions) {
  // User-defined symbols
  assert_parsed_expression_is("a", Symbol::Builder("a", 1));
  assert_parsed_expression_is("x", Symbol::Builder("x", 1));
  assert_parsed_expression_is("toot", Symbol::Builder("toot", 4));
  assert_parsed_expression_is("toto_", Symbol::Builder("toto_", 5));
  assert_parsed_expression_is("t_toto", Symbol::Builder("t_toto", 6));
  assert_parsed_expression_is("tot12", Symbol::Builder("tot12", 5));
  assert_parsed_expression_is("TOto", Symbol::Builder("TOto", 4));
  assert_parsed_expression_is("TO12_Or", Symbol::Builder("TO12_Or", 7));
  assert_text_not_parsable("_a");
  assert_text_not_parsable("abcdefgh");

  // User-defined functions
  assert_parsed_expression_is("f(x)", Function::Builder("f", 1, Symbol::Builder("x",1)));
  assert_parsed_expression_is("f(1)", Function::Builder("f", 1, Rational::Builder(1)));
  assert_parsed_expression_is("ab12AB_(x)", Function::Builder("ab12AB_", 7, Symbol::Builder("x",1)));
  assert_parsed_expression_is("ab12AB_(1)", Function::Builder("ab12AB_", 7, Rational::Builder(1)));
  assert_parsed_expression_is("f(g(x))", Function::Builder("f", 1, Function::Builder("g", 1, Symbol::Builder("x",1))));
  assert_parsed_expression_is("f(g(1))", Function::Builder("f", 1, Function::Builder("g", 1, Rational::Builder(1))));
  assert_parsed_expression_is("f((1))", Function::Builder("f", 1, Parenthesis::Builder(Rational::Builder(1))));
  assert_text_not_parsable("f(1,2)");
  assert_text_not_parsable("f(f)");
  assert_text_not_parsable("abcdefgh(1)");

  // Reserved symbols
  assert_parsed_expression_is("ans", Symbol::Builder("ans", 3));
  assert_parsed_expression_is("𝐢", Constant::Builder(UCodePointMathematicalBoldSmallI));
  assert_parsed_expression_is("π", Constant::Builder(UCodePointGreekSmallLetterPi));
  assert_parsed_expression_is("ℯ", Constant::Builder(UCodePointScriptSmallE));
  assert_parsed_expression_is(Infinity::Name(), Infinity::Builder(false));
  assert_parsed_expression_is(Undefined::Name(), Undefined::Builder());

  assert_text_not_parsable("u");
  assert_text_not_parsable("v");

  // Reserved functions
  assert_parsed_expression_is("acos(1)", ArcCosine::Builder(Rational::Builder(1)));
  assert_parsed_expression_is("acosh(1)", HyperbolicArcCosine::Builder(Rational::Builder(1)));
  assert_parsed_expression_is("abs(1)", AbsoluteValue::Builder(Rational::Builder(1)));
  assert_parsed_expression_is("arg(1)", ComplexArgument::Builder(Rational::Builder(1)));
  assert_parsed_expression_is("asin(1)", ArcSine::Builder(Rational::Builder(1)));
  assert_parsed_expression_is("asinh(1)", HyperbolicArcSine::Builder(Rational::Builder(1)));
  assert_parsed_expression_is("atan(1)", ArcTangent::Builder(Rational::Builder(1)));
  assert_parsed_expression_is("atanh(1)", HyperbolicArcTangent::Builder(Rational::Builder(1)));
  assert_parsed_expression_is("binomial(2,1)", BinomialCoefficient::Builder(Rational::Builder(2),Rational::Builder(1)));
  assert_parsed_expression_is("ceil(1)", Ceiling::Builder(Rational::Builder(1)));
  assert_parsed_expression_is("confidence(1,2)", ConfidenceInterval::Builder(Rational::Builder(1),Rational::Builder(2)));
  assert_text_not_parsable("diff(1,2,3)");
  assert_parsed_expression_is("diff(1,x,3)", Derivative::Builder(Rational::Builder(1),Symbol::Builder("x",1),Rational::Builder(3)));
  assert_parsed_expression_is("dim(1)", MatrixDimension::Builder(Rational::Builder(1)));
  assert_parsed_expression_is("conj(1)", Conjugate::Builder(Rational::Builder(1)));
  assert_parsed_expression_is("det(1)", Determinant::Builder(Rational::Builder(1)));
  assert_parsed_expression_is("cos(1)", Cosine::Builder(Rational::Builder(1)));
  assert_parsed_expression_is("cosh(1)", HyperbolicCosine::Builder(Rational::Builder(1)));
  assert_parsed_expression_is("factor(1)", Factor::Builder(Rational::Builder(1)));
  assert_parsed_expression_is("floor(1)", Floor::Builder(Rational::Builder(1)));
  assert_parsed_expression_is("frac(1)", FracPart::Builder(Rational::Builder(1)));
  assert_parsed_expression_is("gcd(1,2)", GreatCommonDivisor::Builder(Rational::Builder(1),Rational::Builder(2)));
  assert_parsed_expression_is("im(1)", ImaginaryPart::Builder(Rational::Builder(1)));
  assert_parsed_expression_is("int(1,x,2,3)", Integral::Builder(Rational::Builder(1),Symbol::Builder("x",1),Rational::Builder(2),Rational::Builder(3)));
  assert_text_not_parsable("int(1,2,3,4)");
  assert_parsed_expression_is("inverse(1)", MatrixInverse::Builder(Rational::Builder(1)));
  assert_parsed_expression_is("lcm(1,2)", LeastCommonMultiple::Builder(Rational::Builder(1),Rational::Builder(2)));
  assert_parsed_expression_is("ln(1)", NaperianLogarithm::Builder(Rational::Builder(1)));
  assert_parsed_expression_is("log(1)", CommonLogarithm::Builder(Rational::Builder(1)));
  assert_parsed_expression_is("log(1,2)", Logarithm::Builder(Rational::Builder(1),Rational::Builder(2)));
  assert_parsed_expression_is("log_{2}(1)", Logarithm::Builder(Rational::Builder(1),Rational::Builder(2)));
  assert_parsed_expression_is("permute(2,1)", PermuteCoefficient::Builder(Rational::Builder(2),Rational::Builder(1)));
  assert_parsed_expression_is("prediction95(1,2)", PredictionInterval::Builder(Rational::Builder(1),Rational::Builder(2)));
  assert_parsed_expression_is("prediction(1,2)", SimplePredictionInterval::Builder(Rational::Builder(1),Rational::Builder(2)));
  assert_parsed_expression_is("product(1,n,2,3)", Product::Builder(Rational::Builder(1),Symbol::Builder("n",1),Rational::Builder(2),Rational::Builder(3)));
  assert_text_not_parsable("product(1,2,3,4)");
  assert_parsed_expression_is("quo(1,2)", DivisionQuotient::Builder(Rational::Builder(1),Rational::Builder(2)));
  assert_parsed_expression_is("random()", Random::Builder());
  assert_parsed_expression_is("randint(1,2)", Randint::Builder(Rational::Builder(1),Rational::Builder(2)));
  assert_parsed_expression_is("re(1)", RealPart::Builder(Rational::Builder(1)));
  assert_parsed_expression_is("rem(1,2)", DivisionRemainder::Builder(Rational::Builder(1),Rational::Builder(2)));
  assert_parsed_expression_is("root(1,2)", NthRoot::Builder(Rational::Builder(1),Rational::Builder(2)));
  assert_parsed_expression_is("round(1,2)", Round::Builder(Rational::Builder(1),Rational::Builder(2)));
  assert_parsed_expression_is("sin(1)", Sine::Builder(Rational::Builder(1)));
  assert_parsed_expression_is("sign(1)", SignFunction::Builder(Rational::Builder(1)));
  assert_parsed_expression_is("sinh(1)", HyperbolicSine::Builder(Rational::Builder(1)));
  assert_parsed_expression_is("sum(1,n,2,3)", Sum::Builder(Rational::Builder(1),Symbol::Builder("n",1),Rational::Builder(2),Rational::Builder(3)));
  assert_text_not_parsable("sum(1,2,3,4)");
  assert_parsed_expression_is("tan(1)", Tangent::Builder(Rational::Builder(1)));
  assert_parsed_expression_is("tanh(1)", HyperbolicTangent::Builder(Rational::Builder(1)));
  assert_parsed_expression_is("trace(1)", MatrixTrace::Builder(Rational::Builder(1)));
  assert_parsed_expression_is("transpose(1)", MatrixTranspose::Builder(Rational::Builder(1)));
  assert_parsed_expression_is("√(1)", SquareRoot::Builder(Rational::Builder(1)));
  assert_text_not_parsable("cos(1,2)");
  assert_text_not_parsable("log(1,2,3)");
}

QUIZ_CASE(poincare_parsing_parse_store) {
  assert_parsed_expression_is("1→a", Store::Builder(Rational::Builder(1),Symbol::Builder("a",1)));
  assert_parsed_expression_is("1→e", Store::Builder(Rational::Builder(1),Symbol::Builder("e",1)));
  assert_parsed_expression_is("1→f(x)", Store::Builder(Rational::Builder(1),Function::Builder("f",1,Symbol::Builder("x",1))));
  assert_parsed_expression_is("x→f(x)", Store::Builder(Symbol::Builder("x",1),Function::Builder("f",1,Symbol::Builder("x",1))));
  assert_parsed_expression_is("n→f(x)", Store::Builder(Symbol::Builder("n",1),Function::Builder("f",1,Symbol::Builder("x",1))));
  Expression m0[] = {Symbol::Builder('x')};
  assert_parsed_expression_is("[[x]]→f(x)", Store::Builder(BuildMatrix(1,1,m0), Function::Builder("f", 1, Symbol::Builder('x'))));
  assert_text_not_parsable("a→b→c");
  assert_text_not_parsable("1→2");
  assert_text_not_parsable("1→");
  assert_text_not_parsable("→2");
  assert_text_not_parsable("(1→a)");
  assert_text_not_parsable("1→u(n)");
  assert_text_not_parsable("1→u(n+1)");
  assert_text_not_parsable("1→v(n)");
  assert_text_not_parsable("1→v(n+1)");
  assert_text_not_parsable("1→u_{n}");
  assert_text_not_parsable("1→u_{n+1}");
  assert_text_not_parsable("1→v_{n}");
  assert_text_not_parsable("1→v_{n+1}");
  assert_text_not_parsable("1→inf");
  assert_text_not_parsable("1→undef");
  assert_text_not_parsable("1→π");
  assert_text_not_parsable("1→𝐢");
  assert_text_not_parsable("1→ℯ");
  assert_text_not_parsable("1→\1"); // UnknownX
  assert_text_not_parsable("1→\2"); // UnknownN
  assert_text_not_parsable("1→acos");
  assert_text_not_parsable("1→f(2)");
  assert_text_not_parsable("1→f(f)");
  assert_text_not_parsable("3→f(g(4))");
  assert_text_not_parsable("1→ans");
  assert_text_not_parsable("ans→ans");
}

QUIZ_CASE(poincare_parsing_implicit_multiplication) {
  assert_text_not_parsable(".1.2");
  assert_text_not_parsable("1 2");
  assert_parsed_expression_is("1x", Multiplication::Builder(Rational::Builder(1),Symbol::Builder("x", 1)));
  assert_parsed_expression_is("1ans", Multiplication::Builder(Rational::Builder(1),Symbol::Builder("ans", 3)));
  assert_parsed_expression_is("x1", Symbol::Builder("x1", 2));
  assert_parsed_expression_is("1x+2", Addition::Builder(Multiplication::Builder(Rational::Builder(1),Symbol::Builder("x", 1)),Rational::Builder(2)));
  assert_parsed_expression_is("1π", Multiplication::Builder(Rational::Builder(1),Constant::Builder(UCodePointGreekSmallLetterPi)));
  assert_parsed_expression_is("1x-2", Subtraction::Builder(Multiplication::Builder(Rational::Builder(1),Symbol::Builder("x", 1)),Rational::Builder(2)));
  assert_parsed_expression_is("-1x", Opposite::Builder(Multiplication::Builder(Rational::Builder(1),Symbol::Builder("x", 1))));
  assert_parsed_expression_is("2×1x", Multiplication::Builder(Rational::Builder(2),Multiplication::Builder(Rational::Builder(1),Symbol::Builder("x", 1))));
  assert_parsed_expression_is("2^1x", Multiplication::Builder(Power::Builder(Rational::Builder(2),Rational::Builder(1)),Symbol::Builder("x", 1)));
  assert_parsed_expression_is("1x^2", Multiplication::Builder(Rational::Builder(1),Power::Builder(Symbol::Builder("x", 1),Rational::Builder(2))));
  assert_parsed_expression_is("2/1x", Division::Builder(Rational::Builder(2),Multiplication::Builder(Rational::Builder(1),Symbol::Builder("x", 1))));
  assert_parsed_expression_is("1x/2", Division::Builder(Multiplication::Builder(Rational::Builder(1),Symbol::Builder("x", 1)),Rational::Builder(2)));
  assert_parsed_expression_is("(1)2", Multiplication::Builder(Parenthesis::Builder(Rational::Builder(1)),Rational::Builder(2)));
  assert_parsed_expression_is("1(2)", Multiplication::Builder(Rational::Builder(1),Parenthesis::Builder(Rational::Builder(2))));
  assert_parsed_expression_is("sin(1)2", Multiplication::Builder(Sine::Builder(Rational::Builder(1)),Rational::Builder(2)));
  assert_parsed_expression_is("1cos(2)", Multiplication::Builder(Rational::Builder(1),Cosine::Builder(Rational::Builder(2))));
  assert_parsed_expression_is("1!2", Multiplication::Builder(Factorial::Builder(Rational::Builder(1)),Rational::Builder(2)));
  assert_parsed_expression_is("2ℯ^(3)", Multiplication::Builder(Rational::Builder(2),Power::Builder(Constant::Builder(UCodePointScriptSmallE),Parenthesis::Builder(Rational::Builder(3)))));
  assert_parsed_expression_is("\u00122^3\u00133", Multiplication::Builder(Power::Builder(Rational::Builder(2),Rational::Builder(3)), Rational::Builder(3)));
  Expression m1[] = {Rational::Builder(1)}; Matrix M1 = BuildMatrix(1,1,m1);
  Expression m2[] = {Rational::Builder(2)}; Matrix M2 = BuildMatrix(1,1,m2);
  assert_parsed_expression_is("[[1]][[2]]", Multiplication::Builder(M1,M2));
}

QUIZ_CASE(poincare_parsing_adding_missing_parentheses) {
  assert_parsed_expression_with_user_parentheses_is("1+-2", Addition::Builder(Rational::Builder(1),Parenthesis::Builder(Opposite::Builder(Rational::Builder(2)))));
  assert_parsed_expression_with_user_parentheses_is("1--2", Subtraction::Builder(Rational::Builder(1),Parenthesis::Builder(Opposite::Builder(Rational::Builder(2)))));
  assert_parsed_expression_with_user_parentheses_is("1+conj(-2)", Addition::Builder(Rational::Builder(1),Parenthesis::Builder(Conjugate::Builder(Opposite::Builder(Rational::Builder(2))))));
  assert_parsed_expression_with_user_parentheses_is("1-conj(-2)", Subtraction::Builder(Rational::Builder(1),Parenthesis::Builder(Conjugate::Builder(Opposite::Builder(Rational::Builder(2))))));
  assert_parsed_expression_with_user_parentheses_is("3conj(1+𝐢)", Multiplication::Builder(Rational::Builder(3), Parenthesis::Builder(Conjugate::Builder(Addition::Builder(Rational::Builder(1), Constant::Builder(UCodePointMathematicalBoldSmallI))))));
  assert_parsed_expression_with_user_parentheses_is("2×-3", Multiplication::Builder(Rational::Builder(2), Parenthesis::Builder(Opposite::Builder(Rational::Builder(3)))));
  assert_parsed_expression_with_user_parentheses_is("2×-3", Multiplication::Builder(Rational::Builder(2), Parenthesis::Builder(Opposite::Builder(Rational::Builder(3)))));
  assert_parsed_expression_with_user_parentheses_is("--2", Opposite::Builder(Parenthesis::Builder(Opposite::Builder(Rational::Builder(2)))));
  assert_parsed_expression_with_user_parentheses_is("\u00122/3\u0013^2", Power::Builder(Parenthesis::Builder(Division::Builder(Rational::Builder(2), Rational::Builder(3))), Rational::Builder(2)));
}
